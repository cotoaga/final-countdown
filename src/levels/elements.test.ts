import { describe, expect, it } from 'vitest';
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import {
  TILE_BYTES,
  TILE_INDEX_MASK,
  decodeElements,
  elementsBankForLevel,
} from './elements';

const LEV = join(__dirname, '..', '..', 'assets', 'raw', 'lev');
const raw = (name: string) => new Uint8Array(readFileSync(join(LEV, name)));

/** Golden counts probed 2026-07-06; stored header == (size−2)/160 for all. */
const GOLDEN_COUNTS: Record<number, number> = { 0: 283, 1: 247, 2: 286, 3: 247 };

describe('decodeElements — all 4 original banks', () => {
  for (let bank = 0; bank < 4; bank++) {
    it(`Elements${bank}: golden count, indices within 5-bitplane range`, () => {
      const bytes = raw(`Elements${bank}`);
      expect((bytes.byteLength - 2) % TILE_BYTES).toBe(0);

      const el = decodeElements(bytes);
      expect(el.count).toBe(GOLDEN_COUNTS[bank]);
      expect(el.tiles.length).toBe(el.count);

      // Every pixel is a 5-plane palette index; a bank of all-zero tiles
      // would mean the planar unpack is broken.
      let nonZero = 0;
      for (const tile of el.tiles) {
        expect(tile.length).toBe(256);
        for (const p of tile) {
          expect(p).toBeLessThan(32);
          if (p !== 0) nonZero++;
        }
      }
      expect(nonZero).toBeGreaterThan(0);
    });
  }

  it('rejects size/count mismatches', () => {
    expect(() => decodeElements(new Uint8Array(161))).toThrow(/2 \+ n\*160/);
    const fake = new Uint8Array(2 + 160);
    fake[1] = 2; // header claims 2 tiles, size says 1
    expect(() => decodeElements(fake)).toThrow(/mismatch/);
  });
});

describe('level → bank map (start.c load_graphics)', () => {
  it('matches the verified group map {0,3}{1,2}{4,5,6}{7}', () => {
    expect([0, 1, 2, 3, 4, 5, 6, 7].map(elementsBankForLevel)).toEqual([
      0, 1, 1, 0, 2, 2, 2, 3,
    ]);
  });
});

describe('tile index mask', () => {
  it('is the trans.asm blit mask', () => {
    expect(TILE_INDEX_MASK).toBe(0x01ff);
    expect(0x8000 & TILE_INDEX_MASK).toBe(0); // MGROUND flag never leaks into the index
  });
});
