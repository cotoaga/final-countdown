/**
 * Ambient deck animation — copper colour cycling, port of cycle_col()
 * (reference/src/main.c:468-499). Called once per tick of the inner loop
 * (main.c:298). There is no tile-swap animation: tiles stay put and the
 * palette moves under them. Gates/lights/trap glow all ride palette slots 11–15.
 * Findings + citations: reference/game-logic.md "Deck animation".
 *
 * `colptr` = value word of COLOR11 in the playfield copper list
 * (start.c:1029-1030: colorptr = copperlist+1; colptr += 2*11 — each copper
 * MOVE is 2 words), so `*(colptr+2k)` is COLOR(11+k). Pure: no DOM.
 *
 * Level 0 only (the one deck that exists; main.c:47-79 holds all eight).
 */

// main.c:48-51 — cyccol[0], VERBATIM: 4 rotation phases × 4 colours (COLOR11–14).
const CYCCOL_L0: ReadonlyArray<readonly number[]> = [
  [0x066, 0x099, 0x0cc, 0x0ff],
  [0x099, 0x0cc, 0x0ff, 0x066],
  [0x0cc, 0x0ff, 0x066, 0x099],
  [0x0ff, 0x066, 0x099, 0x0cc],
];

export const CYCLE_FIRST = 11; // start.c:1030
export const CYCLE_LAST = 15; // COLOR15 = *(colptr+8), the ramp (main.c:489-491)

export interface Cycle {
  calls: number; // cycle_col() invocations so far
  count: number; // main.c:473 `count`: phase advances when it passes 4 → every 5th call
  phase: number; // colcounter3 — next cyccol row to write (main.c:482-483)
  ramp: number; // colcounter, 0..31 ping-pong (main.c:485-488)
  dir: 1 | -1; // coldir
}

export const initCycle = (): Cycle => ({ calls: 0, count: 0, phase: 0, ramp: 0, dir: 1 });

/** One cycle_col() call. */
export function stepCycle(c: Cycle): Cycle {
  let { count, phase, ramp, dir } = c;
  if (++count > 4) {
    count = 0;
    phase = (phase + 1) & 3; // writes row `phase` (see cycledPalette), then ++colcounter3 &= 3
  }
  if (dir > 0) ++ramp;
  else --ramp;
  if (ramp > 2 * 15) dir = -1;
  if (ramp < 1) dir = 1;
  return { calls: c.calls + 1, count, phase, ramp, dir };
}

/**
 * The 32-colour palette the copper list holds after `c`. Slots outside 11–15
 * are the level's Struct colortable untouched. Before the 5th call COLOR11–14
 * still hold the Struct values; COLOR15 is overwritten from the 1st call.
 */
export function cycledPalette(base: readonly number[], c: Cycle): number[] {
  const pal = base.slice();
  if (c.calls >= 5) CYCCOL_L0[(c.phase + 3) & 3].forEach((v, k) => (pal[CYCLE_FIRST + k] = v)); // row written last
  if (c.calls >= 1) pal[CYCLE_LAST] = (c.ramp >> 1) << 8; // main.c:491 (levels 0–3: red ramp)
  return pal;
}
