/**
 * Deck loading + tile-sheet pre-render for runtime use (play page).
 *
 * Second use of what src/designer/decks/main.ts does inline (first use was
 * the viewer; this extraction is the "abstraction on second use" moment).
 * The viewer still carries its own copy — migrating it is a flagged follow-up,
 * not a drive-by in COT-252.
 *
 * Banks are shared across level groups, palettes are not (start.c
 * load_graphics), so the sheet is always built with the LEVEL's colortable.
 */
import { type DeckMatrix, type DeckStruct, decodeMatrix, decodeStruct } from './decoder';
import { type ElementsBank, decodeElements, elementsBankForLevel } from './elements';

export const TILE = 16;

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
const elementsUrls = import.meta.glob('/assets/raw/lev/Elements*', {
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

export interface Deck {
  level: number;
  matrix: DeckMatrix;
  struct: DeckStruct;
  elements: ElementsBank;
}

export async function loadDeck(level: number): Promise<Deck> {
  const [m, s, e] = await Promise.all([
    fetchBytes(urlFor(matrixUrls, 'Matrix', level)),
    fetchBytes(urlFor(structUrls, 'Struct', level)),
    fetchBytes(urlFor(elementsUrls, 'Elements', elementsBankForLevel(level))),
  ]);
  return { level, matrix: decodeMatrix(m), struct: decodeStruct(s), elements: decodeElements(e) };
}

/** Amiga 12-bit 0x0RGB → [r,g,b] nibble-doubled (×17 == duplicate nibble). */
export function amigaRgb(c: number): [number, number, number] {
  return [((c >> 8) & 15) * 17, ((c >> 4) & 15) * 17, (c & 15) * 17];
}

/** Horizontal strip of every tile in the bank, coloured with `colortable`. */
export function buildTileSheet(bank: ElementsBank, colortable: number[]): HTMLCanvasElement {
  const cv = document.createElement('canvas');
  cv.width = bank.count * TILE;
  cv.height = TILE;
  const c2 = cv.getContext('2d')!;
  const img = c2.createImageData(cv.width, TILE);
  const pal = colortable.map(amigaRgb);
  bank.tiles.forEach((tile, t) => {
    for (let y = 0; y < TILE; y++) {
      for (let x = 0; x < TILE; x++) {
        const rgb = pal[tile[y * TILE + x]] ?? [255, 0, 255];
        const o = (y * cv.width + t * TILE + x) * 4;
        img.data[o] = rgb[0];
        img.data[o + 1] = rgb[1];
        img.data[o + 2] = rgb[2];
        img.data[o + 3] = 255;
      }
    }
  });
  c2.putImageData(img, 0, 0);
  return cv;
}
