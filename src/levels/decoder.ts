/**
 * Level data decoder — Struct{N}/Matrix{N} binaries → typed deck data.
 *
 * Format ground truth: reference/src/start.c (load_struct, load_matrix) and
 * reference/src/externals.c (MAXDEVS 32, MAXCOMPS 16, MAXFOES 32).
 * Aztec/Manx C on 68000: int = 16-bit big-endian signed, long = 32-bit BE
 * signed, structs packed with no padding (every member 2-byte aligned).
 *
 * Struct file layout — 5220 bytes, size verified against all 8 originals:
 *
 *   offset  segment     shape                                          bytes
 *   0       exitpos     6 × ExitDef  { x, y, a : i16 }                    36
 *   36      comp        16 × CompDef { x,y,typ,files : i16;
 *                                      accessmode : i16[32] }           1152
 *   1188    device      32 × DevDef  { x,y,on,typ,spec,next : i16;
 *                                      xl1,xr1,xl2,xr2 : i32 }           896
 *   2084    compfiles   16 × 8 × char[20]                               2560
 *   4644    foedef      32 × FoeDef  { txl,txr : i32; y,v,img,mode:i16 } 512
 *   5156    colortable  32 × u16 Amiga 12-bit RGB                         64
 *                                                                Σ = 5220
 *
 * Matrix file layout — 3072 bytes = 12 rows × 128 cols × u16 BE.
 * (In-memory buffer is matrix[15][128]; load_matrix zeroes rows 12–14
 * after reading, so the FILE holds exactly 12 rows.)
 *
 * Matrix cell semantics are only partially mapped: main.c masks cells with
 * 0xC7FF when placing/clearing entities, i.e. bits 11–13 carry an entity
 * number and the remaining bits the tile/flags. Cells are exposed raw here;
 * the render pass pins the split when the tile pipeline lands. Do not guess.
 *
 * Element count trims mirror start.c exactly:
 *   foes = 0; while (foedef[foes].txl) ++foes;   (etc. for devs/comps)
 */

export const MAX_COMPS = 16;
export const MAX_DEVS = 32;
export const MAX_FOES = 32;
export const MAX_EXITS = 6;
export const COMP_FILE_SLOTS = 8;
export const COMP_FILE_LEN = 20;

export const STRUCT_BYTES = 5220;
export const MATRIX_BYTES = 3072;
export const MATRIX_ROWS = 12;
export const MATRIX_COLS = 128;

export interface ExitDef {
  x: number;
  y: number;
  a: number;
}

export interface CompDef {
  x: number;
  y: number;
  typ: number;
  files: number;
  accessmode: number[]; // i16[MAX_DEVS] — per-device access rights
  fileNames: string[]; // trimmed, empty slots removed
}

export interface DevDef {
  x: number;
  y: number;
  on: number;
  typ: number;
  spec: number;
  next: number;
  xl1: number;
  xr1: number;
  xl2: number;
  xr2: number;
}

export interface FoeDef {
  txl: number;
  txr: number;
  y: number;
  v: number;
  img: number;
  mode: number;
}

export interface DeckStruct {
  exits: ExitDef[]; // fixed 6, as stored
  comps: CompDef[]; // sentinel-trimmed (x === 0), like start.c
  devices: DevDef[];
  foes: FoeDef[];
  colortable: number[]; // 32 × Amiga 12-bit 0x0RGB
}

export interface DeckMatrix {
  rows: number;
  cols: number;
  /** Row-major raw cells, rows*cols entries, u16 each. */
  cells: Uint16Array;
}

function view(bytes: Uint8Array): DataView {
  return new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
}

function readAscii(bytes: Uint8Array, offset: number, len: number): string {
  let end = offset;
  const stop = offset + len;
  while (end < stop && bytes[end] !== 0) end++;
  let s = '';
  for (let i = offset; i < end; i++) s += String.fromCharCode(bytes[i]);
  return s.trim();
}

export function decodeMatrix(bytes: Uint8Array): DeckMatrix {
  if (bytes.byteLength !== MATRIX_BYTES) {
    throw new Error(`Matrix file must be ${MATRIX_BYTES} bytes, got ${bytes.byteLength}`);
  }
  const dv = view(bytes);
  const cells = new Uint16Array(MATRIX_ROWS * MATRIX_COLS);
  for (let i = 0; i < cells.length; i++) cells[i] = dv.getUint16(i * 2, false);
  return { rows: MATRIX_ROWS, cols: MATRIX_COLS, cells };
}

export function matrixCell(m: DeckMatrix, row: number, col: number): number {
  return m.cells[row * m.cols + col];
}

export function decodeStruct(bytes: Uint8Array): DeckStruct {
  if (bytes.byteLength !== STRUCT_BYTES) {
    throw new Error(`Struct file must be ${STRUCT_BYTES} bytes, got ${bytes.byteLength}`);
  }
  const dv = view(bytes);
  let o = 0;

  const exits: ExitDef[] = [];
  for (let i = 0; i < MAX_EXITS; i++) {
    exits.push({ x: dv.getInt16(o, false), y: dv.getInt16(o + 2, false), a: dv.getInt16(o + 4, false) });
    o += 6;
  }

  const rawComps: Omit<CompDef, 'fileNames'>[] = [];
  for (let i = 0; i < MAX_COMPS; i++) {
    const x = dv.getInt16(o, false);
    const y = dv.getInt16(o + 2, false);
    const typ = dv.getInt16(o + 4, false);
    const files = dv.getInt16(o + 6, false);
    const accessmode: number[] = [];
    for (let j = 0; j < MAX_DEVS; j++) accessmode.push(dv.getInt16(o + 8 + j * 2, false));
    rawComps.push({ x, y, typ, files, accessmode });
    o += 8 + MAX_DEVS * 2;
  }

  const devices: DevDef[] = [];
  for (let i = 0; i < MAX_DEVS; i++) {
    devices.push({
      x: dv.getInt16(o, false),
      y: dv.getInt16(o + 2, false),
      on: dv.getInt16(o + 4, false),
      typ: dv.getInt16(o + 6, false),
      spec: dv.getInt16(o + 8, false),
      next: dv.getInt16(o + 10, false),
      xl1: dv.getInt32(o + 12, false),
      xr1: dv.getInt32(o + 16, false),
      xl2: dv.getInt32(o + 20, false),
      xr2: dv.getInt32(o + 24, false),
    });
    o += 28;
  }

  const fileNames: string[][] = [];
  for (let i = 0; i < MAX_COMPS; i++) {
    const names: string[] = [];
    for (let j = 0; j < COMP_FILE_SLOTS; j++) {
      const s = readAscii(bytes, o, COMP_FILE_LEN);
      if (s.length > 0) names.push(s);
      o += COMP_FILE_LEN;
    }
    fileNames.push(names);
  }

  const foes: FoeDef[] = [];
  for (let i = 0; i < MAX_FOES; i++) {
    foes.push({
      txl: dv.getInt32(o, false),
      txr: dv.getInt32(o + 4, false),
      y: dv.getInt16(o + 8, false),
      v: dv.getInt16(o + 10, false),
      img: dv.getInt16(o + 12, false),
      mode: dv.getInt16(o + 14, false),
    });
    o += 16;
  }

  const colortable: number[] = [];
  for (let i = 0; i < 32; i++) {
    colortable.push(dv.getUint16(o, false));
    o += 2;
  }

  // Sentinel trims, exactly as start.c derives foes/devs/comps.
  let nComps = 0;
  while (nComps < MAX_COMPS && rawComps[nComps].x !== 0) nComps++;
  let nDevs = 0;
  while (nDevs < MAX_DEVS && devices[nDevs].x !== 0) nDevs++;
  let nFoes = 0;
  while (nFoes < MAX_FOES && foes[nFoes].txl !== 0) nFoes++;

  const comps: CompDef[] = rawComps
    .slice(0, nComps)
    .map((c, i) => ({ ...c, fileNames: fileNames[i] }));

  return {
    exits,
    comps,
    devices: devices.slice(0, nDevs),
    foes: foes.slice(0, nFoes),
    colortable,
  };
}

/** Amiga 12-bit 0x0RGB → CSS #rrggbb (nibble-doubled, the hardware way). */
export function amigaColorToCss(c: number): string {
  const r = (c >> 8) & 0xf;
  const g = (c >> 4) & 0xf;
  const b = c & 0xf;
  const h = (n: number) => ((n << 4) | n).toString(16).padStart(2, '0');
  return `#${h(r)}${h(g)}${h(b)}`;
}
