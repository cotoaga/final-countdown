import { describe, expect, it } from 'vitest';
import { readFileSync } from 'node:fs';
import { join } from 'node:path';
import {
  MATRIX_BYTES,
  MATRIX_COLS,
  MATRIX_ROWS,
  STRUCT_BYTES,
  amigaColorToCss,
  decodeMatrix,
  decodeStruct,
  matrixCell,
} from './decoder';

const LEV = join(__dirname, '..', '..', 'assets', 'raw', 'lev');

const raw = (name: string) => new Uint8Array(readFileSync(join(LEV, name)));

/**
 * Golden counts probed from the original binaries (2026-07-05) using the
 * exact sentinel logic of start.c. If these move, either the assets changed
 * or the decoder broke — both are stop-the-line events.
 */
const GOLDEN: Record<number, { comps: number; devs: number; foes: number; firstFile: string }> = {
  0: { comps: 12, devs: 15, foes: 9, firstFile: 'DECK1.TXT' },
  1: { comps: 13, devs: 24, foes: 8, firstFile: 'DECK3.TXT' },
  2: { comps: 11, devs: 16, foes: 8, firstFile: 'DECK2.TXT' },
  3: { comps: 4, devs: 2, foes: 0, firstFile: 'DECK3.TXT' },
  4: { comps: 7, devs: 19, foes: 11, firstFile: 'SECTION2.TXT' },
  5: { comps: 10, devs: 14, foes: 8, firstFile: 'DECK5.TXT' },
  6: { comps: 11, devs: 21, foes: 6, firstFile: 'LAGER.TXT' },
  7: { comps: 15, devs: 15, foes: 5, firstFile: '.DAYTKOR.TXT' },
};

describe('decodeStruct — all 8 original Struct files', () => {
  for (let lvl = 0; lvl < 8; lvl++) {
    it(`Struct${lvl}: size, golden counts, sane content`, () => {
      const bytes = raw(`Struct${lvl}`);
      expect(bytes.byteLength).toBe(STRUCT_BYTES);

      const deck = decodeStruct(bytes);
      const g = GOLDEN[lvl];

      expect(deck.comps.length).toBe(g.comps);
      expect(deck.devices.length).toBe(g.devs);
      expect(deck.foes.length).toBe(g.foes);

      // First terminal of every deck carries readable ASCII filenames.
      if (g.comps > 0) {
        expect(deck.comps[0].fileNames[0]).toBe(g.firstFile);
        for (const c of deck.comps) {
          for (const f of c.fileNames) {
            expect(f).toMatch(/^[\x20-\x7e]+$/);
            expect(f.length).toBeLessThanOrEqual(20);
          }
          expect(c.accessmode.length).toBe(32);
        }
      }

      // Palette: 32 entries, all valid Amiga 12-bit values.
      expect(deck.colortable.length).toBe(32);
      for (const c of deck.colortable) {
        expect(c).toBeGreaterThanOrEqual(0);
        expect(c).toBeLessThanOrEqual(0x0fff);
      }

      expect(deck.exits.length).toBe(6);
    });
  }

  it('rejects wrong-size input', () => {
    expect(() => decodeStruct(new Uint8Array(5219))).toThrow(/5220/);
  });
});

describe('decodeMatrix — all 8 original Matrix files', () => {
  for (let lvl = 0; lvl < 8; lvl++) {
    it(`Matrix${lvl}: 12×128 grid of u16`, () => {
      const bytes = raw(`Matrix${lvl}`);
      expect(bytes.byteLength).toBe(MATRIX_BYTES);

      const m = decodeMatrix(bytes);
      expect(m.rows).toBe(MATRIX_ROWS);
      expect(m.cols).toBe(MATRIX_COLS);
      expect(m.cells.length).toBe(MATRIX_ROWS * MATRIX_COLS);

      // A playfield that is entirely empty or entirely solid is a decode bug.
      const nonZero = m.cells.reduce((n, c) => n + (c !== 0 ? 1 : 0), 0);
      expect(nonZero).toBeGreaterThan(0);
      expect(nonZero).toBeLessThan(m.cells.length);

      expect(matrixCell(m, 0, 0)).toBe(m.cells[0]);
      expect(matrixCell(m, MATRIX_ROWS - 1, MATRIX_COLS - 1)).toBe(m.cells[m.cells.length - 1]);
    });
  }

  it('rejects wrong-size input', () => {
    expect(() => decodeMatrix(new Uint8Array(3840))).toThrow(/3072/);
  });
});

describe('amigaColorToCss', () => {
  it('nibble-doubles like the hardware', () => {
    expect(amigaColorToCss(0x000)).toBe('#000000');
    expect(amigaColorToCss(0xfff)).toBe('#ffffff');
    expect(amigaColorToCss(0xf00)).toBe('#ff0000');
    expect(amigaColorToCss(0x256)).toBe('#225566');
  });
});
