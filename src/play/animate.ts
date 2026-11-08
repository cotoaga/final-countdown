/**
 * Live-palette painter: the tile sheet and item strip, recoloured in place
 * whenever the cycled palette (cycle.ts) changes. Only tiles/objects that
 * actually use slots 11–15 are repainted — everything else is palette-static.
 */
import { TILE, buildTileSheet, amigaRgb } from '../levels/load';
import type { ElementsBank } from '../levels/elements';
import { CYCLE_FIRST, CYCLE_LAST } from './cycle';
import type { ItemBank } from './items';

const usesCycled = (px: Uint8Array) => px.some((v) => v >= CYCLE_FIRST && v <= CYCLE_LAST);

export interface DeckPaint {
  sheet: HTMLCanvasElement; // one tile per TILE px, index = tile number
  items: HTMLCanvasElement; // one object per TILE px, index = itemOf(cell)
  recolour(palette: readonly number[]): void;
}

export function createDeckPaint(bank: ElementsBank, items: ItemBank, base: readonly number[]): DeckPaint {
  const sheet = buildTileSheet(bank, base.slice());
  const strip = document.createElement('canvas');
  strip.width = items.pix.length * TILE;
  strip.height = TILE;
  const sctx = sheet.getContext('2d')!;
  const ictx = strip.getContext('2d')!;
  const tiles = Array.from(bank.tiles.keys()).filter((i) => usesCycled(bank.tiles[i]));
  const img = new ImageData(TILE, TILE);
  let last = '';

  const paint = (ctx: CanvasRenderingContext2D, x: number, px: Uint8Array, pal: Array<[number, number, number]>, alpha?: Uint8Array) => {
    for (let i = 0; i < px.length; i++) {
      const [r, g, b] = pal[px[i]];
      img.data.set([r, g, b, alpha && !alpha[i] ? 0 : 255], i * 4);
    }
    ctx.putImageData(img, x, 0);
  };

  const recolour = (palette: readonly number[]) => {
    const key = palette.slice(CYCLE_FIRST, CYCLE_LAST + 1).join();
    if (key === last) return;
    last = key;
    const pal = palette.map(amigaRgb);
    for (const t of tiles) paint(sctx, t * TILE, bank.tiles[t], pal);
    items.pix.forEach((px, i) => paint(ictx, i * TILE, px, pal, items.opaque[i]));
  };
  recolour(base);
  return { sheet, items: strip, recolour };
}
