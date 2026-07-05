// Sandbox level: a flat floor with exactly one jetpack-mandatory gap. This is
// sandbox scaffolding, not canon — via negativa: no Matrix/level loading here,
// just enough geometry to feel the jetpack against a fall. Screen mapping
// (screen_x = xp/32, screen_y = yp/64) follows reference/move-physics.md.
import type { FlightBody } from "./physics";

export interface FlightWorld {
  floorYp: number;
  fallLimitYp: number; // yp past which, over the gap, is a fall -> respawn
  gapStartXp: number;
  gapEndXp: number;
  minXp: number;
  maxXp: number;
  spawnXp: number;
  spawnYp: number;
}

const FLOOR_SCREEN_Y = 170;
const GAP_START_SCREEN_X = 150;
const GAP_END_SCREEN_X = 190;

export function makeWorld(): FlightWorld {
  const floorYp = FLOOR_SCREEN_Y * 64;
  return {
    floorYp,
    fallLimitYp: floorYp + 8 * 64, // small grace so the fall is visible, not a teleport
    gapStartXp: GAP_START_SCREEN_X * 32,
    gapEndXp: GAP_END_SCREEN_X * 32,
    minXp: 0,
    maxXp: 310 * 32,
    spawnXp: 20 * 32,
    spawnYp: floorYp,
  };
}

export function isOverGap(xp: number, w: FlightWorld): boolean {
  return xp >= w.gapStartXp && xp <= w.gapEndXp;
}

/** Ground collision, gap fall-through, and screen bounds. Pure. */
export function applyWorld(b: FlightBody, w: FlightWorld): FlightBody {
  let { xp, yp, velx, vely, fuel } = b;
  xp = clamp(xp, w.minXp, w.maxXp);

  if (isOverGap(xp, w) && yp >= w.fallLimitYp) {
    return { xp: w.spawnXp, yp: w.spawnYp, velx: 0, vely: 0, fuel };
  }
  if (!isOverGap(xp, w) && yp >= w.floorYp) {
    yp = w.floorYp;
    vely = 0;
  }
  return { xp, yp, velx, vely, fuel };
}

function clamp(v: number, lo: number, hi: number): number {
  return v < lo ? lo : v > hi ? hi : v;
}
