import { readFileSync } from 'node:fs';
import { describe, expect, it } from 'vitest';
import { cycledPalette, initCycle, stepCycle } from './cycle';
import { decodeItems, itemOf } from './items';

const base = Array.from({ length: 32 }, (_, i) => i);
const after = (n: number) => {
  let c = initCycle();
  for (let i = 0; i < n; i++) c = stepCycle(c);
  return c;
};
const slots = (n: number) => cycledPalette(base, after(n)).slice(11, 16);

describe('cycle_col — main.c:468', () => {
  it('COLOR11–14 untouched until the 5th call, then cyccol[0] row 0, 1, 2, 3 every 5th call', () => {
    expect(slots(4).slice(0, 4)).toEqual([11, 12, 13, 14]);
    expect(slots(5).slice(0, 4)).toEqual([0x066, 0x099, 0x0cc, 0x0ff]);
    expect(slots(9).slice(0, 4)).toEqual([0x066, 0x099, 0x0cc, 0x0ff]);
    expect(slots(10).slice(0, 4)).toEqual([0x099, 0x0cc, 0x0ff, 0x066]);
    expect(slots(15).slice(0, 4)).toEqual([0x0cc, 0x0ff, 0x066, 0x099]);
    expect(slots(20).slice(0, 4)).toEqual([0x0ff, 0x066, 0x099, 0x0cc]);
    expect(slots(25).slice(0, 4)).toEqual([0x066, 0x099, 0x0cc, 0x0ff]); // wraps &= 3
  });

  it('COLOR15 is a red ping-pong ramp, (colcounter>>1)<<8, turning at 31 and at 0', () => {
    expect(slots(0)[4]).toBe(15); // not yet overwritten
    expect(slots(1)[4]).toBe(0x000); // colcounter 1
    expect(slots(2)[4]).toBe(0x100); // colcounter 2
    expect(slots(30)[4]).toBe(0xf00); // colcounter 30
    expect(slots(31)[4]).toBe(0xf00); // colcounter 31: >30 → dir = -1
    expect(slots(32)[4]).toBe(0xf00); // 30
    expect(slots(33)[4]).toBe(0xe00); // 29
    expect(after(62).ramp).toBe(0); // <1 → dir = +1
    expect(after(63).ramp).toBe(1);
    expect(after(124).ramp).toBe(0); // full period 62 ticks
  });

  it('other slots are never touched', () => {
    const p = cycledPalette(base, after(37));
    expect(p.filter((v, i) => v !== i).length).toBeLessThanOrEqual(5);
    expect(p.slice(0, 11)).toEqual(base.slice(0, 11));
    expect(p.slice(16)).toEqual(base.slice(16));
  });
});

describe('item bobs — gfx/Stuff via _copy_obj', () => {
  const bank = decodeItems(new Uint8Array(readFileSync('assets/raw/gfx-release/Stuff')));
  const opaqueCount = (i: number) => bank.opaque[i].reduce((a, b) => a + b, 0);
  const cycled = (i: number) => bank.pix[i].filter((v, k) => bank.opaque[i][k] && v >= 11 && v <= 15).length;

  it('cell item field → object: MMINE 0, MTRAP 1, MFUEL 2, MDISK1 3, MDISK2+ none', () => {
    expect([0x0800, 0x1000, 0x1800, 0x2000].map((b) => itemOf(b | 0x8005))).toEqual([0, 1, 2, 3]);
    expect([0, 0x2800, 0x3000, 0x3800].map(itemOf)).toEqual([-1, -1, -1, -1]);
  });

  it('mine.wait (obj 0) is static; mine.active (obj 1) glows through the cycled slots', () => {
    expect(opaqueCount(0)).toBe(120);
    expect(opaqueCount(1)).toBe(120);
    expect(cycled(0)).toBe(0);
    expect(cycled(1)).toBe(30); // 4+4+5+3+14 pixels on slots 11–15
    // centre of mine.wait is idx 3/2, the same pixels mine.active draws in 11–15.
    expect(bank.pix[0][10 * 16 + 9]).toBe(3);
    expect(bank.pix[1][10 * 16 + 9]).toBe(11);
  });
});
