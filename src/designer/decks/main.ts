/**
 * Deck viewer — designer tool. Renders decoded Matrix/Struct data for all
 * 8 levels: tile grid in structural false-color, entity overlays in true
 * coordinates, hover/click inspector.
 *
 * FALSE-COLOR, NOT GRAPHICS: real tile pixels live in the Elements banks and
 * arrive with the tile render pass. Here, equal cell values get equal colors
 * drawn from the deck's own 32-color palette, so *structure* is faithful even
 * though *appearance* is not.
 *
 * Coordinate ground truth (probed from Struct binaries, consistent with
 * start.c/move.c units):
 *   comps/devices/exits — TILE coords (x 0..127, y 0..11; floors at 0/4/8)
 *   foes                — txl/txr in 1/32-px, y in 1/64-px (physics units)
 */
import {
  type DeckMatrix,
  type DeckStruct,
  MATRIX_COLS,
  MATRIX_ROWS,
  amigaColorToCss,
  decodeMatrix,
  decodeStruct,
  matrixCell,
} from '../../levels/decoder';

const TILE = 16;

const matrixUrls = import.meta.glob('/assets/raw/lev/Matrix*', {
  query: '?url',
  import: 'default',
  eager: true,
}) as Record<string, string>;
const structUrls = import.meta.glob('/assets/raw/lev/Struct*', {
  query: '?url',
  import: 'default',
  eager: true,
}) as Record<string, string>;

function urlFor(map: Record<string, string>, prefix: string, level: number): string {
  const key = Object.keys(map).find((k) => k.endsWith(`${prefix}${level}`));
  if (!key) throw new Error(`Missing asset ${prefix}${level}`);
  return map[key];
}

async function fetchBytes(url: string): Promise<Uint8Array> {
  const res = await fetch(url);
  if (!res.ok) throw new Error(`Fetch failed: ${url} (${res.status})`);
  return new Uint8Array(await res.arrayBuffer());
}

interface Deck {
  matrix: DeckMatrix;
  struct: DeckStruct;
}

const cache = new Map<number, Deck>();

async function loadDeck(level: number): Promise<Deck> {
  const hit = cache.get(level);
  if (hit) return hit;
  const [m, s] = await Promise.all([
    fetchBytes(urlFor(matrixUrls, 'Matrix', level)),
    fetchBytes(urlFor(structUrls, 'Struct', level)),
  ]);
  const deck = { matrix: decodeMatrix(m), struct: decodeStruct(s) };
  cache.set(level, deck);
  return deck;
}

/** Stable false-color: equal cell values → equal palette picks (skip color 0). */
function cellColor(value: number, palette: string[]): string {
  const idx = 1 + ((value * 2654435761) >>> 0) % (palette.length - 1);
  return palette[idx];
}

function drawDeck(ctx: CanvasRenderingContext2D, deck: Deck): void {
  const { matrix, struct } = deck;
  const palette = struct.colortable.map(amigaColorToCss);

  ctx.fillStyle = '#000';
  ctx.fillRect(0, 0, MATRIX_COLS * TILE, MATRIX_ROWS * TILE);

  for (let r = 0; r < matrix.rows; r++) {
    for (let c = 0; c < matrix.cols; c++) {
      const v = matrixCell(matrix, r, c);
      if (v === 0) continue;
      ctx.fillStyle = cellColor(v, palette);
      ctx.fillRect(c * TILE, r * TILE, TILE - 1, TILE - 1);
    }
  }

  // Foe patrol zones — physics units → px.
  for (const f of struct.foes) {
    const x0 = f.txl / 32;
    const x1 = f.txr / 32;
    const y = f.y / 64;
    ctx.fillStyle = 'rgba(255, 0, 64, 0.35)';
    ctx.fillRect(x0, y, x1 - x0 + TILE, TILE);
    ctx.strokeStyle = '#ff4060';
    ctx.strokeRect(x0 + 0.5, y + 0.5, x1 - x0 + TILE - 1, TILE - 1);
  }

  ctx.font = 'bold 10px monospace';
  ctx.textBaseline = 'top';

  for (const e of struct.exits) {
    if (e.x === 0 && e.y === 0 && e.a === 0) continue; // empty slot
    badge(ctx, e.x, e.y, '#e0e0ff', '#000', 'E');
  }
  for (const d of struct.devices) {
    badge(ctx, d.x, d.y, d.on ? '#20d060' : '#d02020', '#000', 'D');
  }
  for (const c of struct.comps) {
    badge(ctx, c.x, c.y, '#ff8a00', '#000', 'C');
  }
}

function badge(
  ctx: CanvasRenderingContext2D,
  tx: number,
  ty: number,
  bg: string,
  ink: string,
  ch: string,
): void {
  const x = tx * TILE;
  const y = ty * TILE;
  ctx.fillStyle = bg;
  ctx.fillRect(x, y, TILE - 1, TILE - 1);
  ctx.fillStyle = ink;
  ctx.fillText(ch, x + 4, y + 3);
}

function describe(deck: Deck, col: number, row: number): string {
  const parts: string[] = [`COL ${col} ROW ${row}`];
  const v = matrixCell(deck.matrix, row, col);
  parts.push(`CELL 0x${v.toString(16).toUpperCase().padStart(4, '0')}`);

  const comp = deck.struct.comps.find((c) => c.x === col && c.y === row);
  if (comp) parts.push(`TERMINAL typ=${comp.typ} files=[${comp.fileNames.join(', ') || '—'}]`);
  const dev = deck.struct.devices.find((d) => d.x === col && d.y === row);
  if (dev) parts.push(`DEVICE typ=${dev.typ} on=${dev.on} spec=${dev.spec} next=${dev.next}`);
  const exit = deck.struct.exits.find((e) => (e.x !== 0 || e.y !== 0) && e.x === col && e.y === row);
  if (exit) parts.push(`EXIT a=${exit.a}`);
  const foe = deck.struct.foes.find(
    (f) => row === Math.floor(f.y / 64 / TILE) && col * TILE >= f.txl / 32 && col * TILE <= f.txr / 32 + TILE,
  );
  if (foe) parts.push(`FOE img=${foe.img} v=${foe.v} mode=${foe.mode}`);

  return parts.join(' · ');
}

async function main(): Promise<void> {
  const canvas = document.querySelector<HTMLCanvasElement>('#deck-canvas');
  const select = document.querySelector<HTMLSelectElement>('#deck');
  const readout = document.querySelector<HTMLParagraphElement>('#readout');
  if (!canvas || !select || !readout) throw new Error('deck viewer: missing DOM');
  const ctx = canvas.getContext('2d');
  if (!ctx) throw new Error('deck viewer: no 2d context');

  let deck = await loadDeck(Number(select.value));
  drawDeck(ctx, deck);

  select.addEventListener('change', async () => {
    deck = await loadDeck(Number(select.value));
    drawDeck(ctx, deck);
    readout.textContent = `LEVEL ${select.value} · ${deck.struct.comps.length} TERMINALS · ${deck.struct.devices.length} DEVICES · ${deck.struct.foes.length} FOES`;
  });

  const tileAt = (ev: MouseEvent): { col: number; row: number } => {
    const rect = canvas.getBoundingClientRect();
    const col = Math.floor(((ev.clientX - rect.left) / rect.width) * MATRIX_COLS);
    const row = Math.floor(((ev.clientY - rect.top) / rect.height) * MATRIX_ROWS);
    return {
      col: Math.min(MATRIX_COLS - 1, Math.max(0, col)),
      row: Math.min(MATRIX_ROWS - 1, Math.max(0, row)),
    };
  };

  canvas.addEventListener('mousemove', (ev) => {
    const { col, row } = tileAt(ev);
    readout.textContent = describe(deck, col, row);
  });

  canvas.addEventListener('click', (ev) => {
    const { col, row } = tileAt(ev);
    const comp = deck.struct.comps.find((c) => c.x === col && c.y === row);
    readout.textContent = comp
      ? `TERMINAL @ ${col},${row} · FILES: ${comp.fileNames.join(' · ') || '(none)'}`
      : describe(deck, col, row);
  });
}

void main();
