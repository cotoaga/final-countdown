/**
 * Elements tile bank decoder — the real 16×16 deck graphics.
 *
 * Format ground truth:
 *   start.c:679  — Read(fh, ground[0], ...) after a 2-byte count `no`
 *   start.c:978  — ground[0] = AllocMem; ground[i] = base + i*160
 *   trans.asm:3  — _copy_ground: tile index = matrix cell & $01FF; source
 *                  advances LINEARLY over 5 planes × 16 rows × 1 word, i.e.
 *                  tiles are PLANE-SEQUENTIAL (16 words plane 0, 16 words
 *                  plane 1, …), NOT row-interleaved.
 *   externals.c  — MAXPLANES 5 → palette indices 0–31, colours from the
 *                  level's own Struct colortable.
 *
 * File layout: u16 BE tile count, then count × 160 bytes.
 * Verified against all four banks (stored count == (size−2)/160 exactly):
 *   Elements0: 283 · Elements1: 247 · Elements2: 286 · Elements3: 247
 *
 * Matrix cell anatomy (externals.c M* defines + main.c set_obj/clr_obj +
 * trans.asm mask):
 *   bits 0–8  (0x01FF) tile index into the bank
 *   bits 9–10 (0x0600) unaccounted — flagged, do not guess
 *   bits 11–13(0x3800) item field (MMINE/MTRAP/MFUEL/MDISK1–4)
 *   bit 14    (0x4000) MENERGY
 *   bit 15    (0x8000) MGROUND (solid)
 *
 * Bank↔level map (start.c load_graphics, verified 2026-07-05):
 *   levels {0,3}→bank 0 · {1,2}→bank 1 · {4,5,6}→bank 2 · {7}→bank 3
 */

export const TILE_BYTES = 160;
export const TILE_SIDE = 16;
export const TILE_PLANES = 5;

export const TILE_INDEX_MASK = 0x01ff; // trans.asm: and.w #$1FF

export interface ElementsBank {
  count: number;
  /** Per tile: 256 palette indices (0–31), row-major 16×16. */
  tiles: Uint8Array[];
}

export function elementsBankForLevel(level: number): number {
  // start.c load_graphics: 7→3, 4..6→2, 1..2→1, else (0,3)→0
  if (level === 7) return 3;
  if (level >= 4) return 2;
  if (level === 1 || level === 2) return 1;
  return 0;
}

export function decodeElements(bytes: Uint8Array): ElementsBank {
  if (bytes.byteLength < 2 || (bytes.byteLength - 2) % TILE_BYTES !== 0) {
    throw new Error(`Elements bank must be 2 + n*${TILE_BYTES} bytes, got ${bytes.byteLength}`);
  }
  const stored = (bytes[0] << 8) | bytes[1];
  const derived = (bytes.byteLength - 2) / TILE_BYTES;
  if (stored !== derived) {
    throw new Error(`Elements count mismatch: header says ${stored}, size says ${derived}`);
  }

  const tiles: Uint8Array[] = [];
  for (let t = 0; t < derived; t++) {
    const base = 2 + t * TILE_BYTES;
    const px = new Uint8Array(TILE_SIDE * TILE_SIDE);
    for (let plane = 0; plane < TILE_PLANES; plane++) {
      for (let row = 0; row < TILE_SIDE; row++) {
        const o = base + plane * TILE_SIDE * 2 + row * 2;
        const word = (bytes[o] << 8) | bytes[o + 1];
        for (let bit = 0; bit < 16; bit++) {
          if (word & (0x8000 >> bit)) px[row * TILE_SIDE + bit] |= 1 << plane;
        }
      }
    }
    tiles.push(px);
  }
  return { count: derived, tiles };
}
