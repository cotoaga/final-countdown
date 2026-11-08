/**
 * Item bobs — the four 16×16 objects in gfx/Stuff (start.c:467-477 load_obj),
 * blitted over a tile by trans.asm _copy_obj. Layout, from those two sources:
 *   gfx/Stuff: sintable (720 B, start.c:462-465), then 4 × 194 B objects
 *   object   : word 0 header · words 1–80 image, 5 planes × 16 rows, plane-
 *              sequential like tiles · words 81–96 mask, one plane. load_obj
 *              XORs the mask with $FFFF and the blit does `and mask; or image`,
 *              so a RAW mask bit of 1 = opaque.
 * Cell → object: _copy_obj takes (cell & $3800) >> 9, subtracts 4 → byte offset
 * into objmem[] (longs): MMINE→0, MTRAP→1, MFUEL→2, MDISK1→3. MDISK2–4 would
 * index past objmem[4] in the original; they are not drawn.
 * Verified on pixels 2026-11-08: obj 0 == obj/mine.wait, obj 1 == obj/mine.active.
 */
import { TILE_SIDE } from '../levels/elements';

const stuffUrls = import.meta.glob('/assets/raw/gfx-release/Stuff*', {
  query: '?url',
  import: 'default',
  eager: true,
}) as Record<string, string>;

const SINTABLE_BYTES = 720; // start.c:464
const OBJ_BYTES = 194; // start.c:473
export const ITEM_COUNT = 4; // start.c:471

export interface ItemBank {
  /** Per object: 256 palette indices, row-major 16×16. */
  pix: Uint8Array[];
  /** Per object: 256 flags, 1 = opaque (raw mask bit). */
  opaque: Uint8Array[];
}

export function decodeItems(bytes: Uint8Array): ItemBank {
  const word = (o: number) => (bytes[o] << 8) | bytes[o + 1];
  const bank: ItemBank = { pix: [], opaque: [] };
  for (let i = 0; i < ITEM_COUNT; i++) {
    const base = SINTABLE_BYTES + i * OBJ_BYTES;
    const pix = new Uint8Array(TILE_SIDE * TILE_SIDE);
    const opaque = new Uint8Array(TILE_SIDE * TILE_SIDE);
    for (let row = 0; row < TILE_SIDE; row++) {
      const mask = word(base + 2 + 160 + row * 2);
      for (let bit = 0; bit < 16; bit++) {
        const m = 0x8000 >> bit;
        if (mask & m) opaque[row * 16 + bit] = 1;
        for (let plane = 0; plane < 5; plane++) {
          if (word(base + 2 + plane * 32 + row * 2) & m) pix[row * 16 + bit] |= 1 << plane;
        }
      }
    }
    bank.pix.push(pix);
    bank.opaque.push(opaque);
  }
  return bank;
}

/** trans.asm _copy_obj: cell item field → object index, or -1 for none/unsupported. */
export function itemOf(cell: number): number {
  const i = ((cell & 0x3800) >> 11) - 1;
  return i >= 0 && i < ITEM_COUNT ? i : -1;
}

export async function loadItems(): Promise<ItemBank> {
  const url = Object.values(stuffUrls)[0];
  if (!url) throw new Error('Missing asset gfx-release/Stuff');
  const res = await fetch(url);
  if (!res.ok) throw new Error(`Fetch failed: ${url} (${res.status})`);
  return decodeItems(new Uint8Array(await res.arrayBuffer()));
}
