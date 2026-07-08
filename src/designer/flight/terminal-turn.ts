// "Turn to use a terminal" — the girltr[] rotation bank (move.c:150-165,
// SMTURNS/SMTURNE; sprite in set_bob:496-517). The real trigger is stick UP
// + fire button while standing at an MCOMPUTER/MSWITCH tile (move.c:361-366)
// — this sandbox has no device tiles, so it's exposed as a direct keypress
// preview instead (see main.ts). ground-states.ts deliberately left girltr
// out of its fence; this module is that fence reopened, on purpose.
//
// The shipped game built girltr[8] from 16 drawn frames (girlshp>>2,
// move-physics.md: "16 drawn, 8 shipped"). This sandbox plays all 16
// (girlshp>>1) for a smoother preview at the SAME total duration: 32 ticks
// turning + 32 turning back.
//
// Frame order (dev source g00..g15, visually confirmed 2026-07-08): g00 is
// front-facing, g15 is full side profile. Default idle is now front-facing
// (Kurt's call, 2026-07-08 — canon's actual SMGROUND-idle pose was
// girlgsr[0], a side lean, not front-facing), so this preview plays
// front(0) -> side(15) -> front(0): the mirror of the original's
// side-walking-up -> front-facing-the-device -> side-walking-away narrative.
export interface TerminalTurnState {
  active: boolean;
  shp: number; // girlshp, 0..31 while active
  closing: boolean; // false = turning away from the player; true = turning back
}

export function initTerminalTurn(): TerminalTurnState {
  return { active: false, shp: 0, closing: false };
}

/** No-op if already mid-turn — matches the original ignoring re-fire mid-animation. */
export function triggerTerminalTurn(s: TerminalTurnState): TerminalTurnState {
  return s.active ? s : { active: true, shp: 0, closing: false };
}

export function stepTerminalTurn(s: TerminalTurnState): TerminalTurnState {
  if (!s.active) return s;
  if (!s.closing) {
    const shp = s.shp + 1;
    return shp > 31 ? { active: true, shp: 31, closing: true } : { active: true, shp, closing: false };
  }
  const shp = s.shp - 1;
  return shp < 0 ? initTerminalTurn() : { active: true, shp, closing: true };
}

export function terminalTurnFrame(s: TerminalTurnState): number {
  return s.shp >> 1; // 0..31 -> 0..15
}
