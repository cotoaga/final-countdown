// Animation cadence — port of set_bob() in move.c, ground/air cases only
// (shoot, lift, teleport, death banks are out of scope for this sandbox).
// girlshp increments every tick and wraps: &63 on the ground (16-frame bank,
// >>2), &15 in the air (4-frame bank, >>2).
export interface AnimState {
  counter: number;
  facing: 1 | -1;
}

export function initAnim(): AnimState {
  return { counter: 0, facing: 1 };
}

export function stepAnim(a: AnimState, velx: number, flying: boolean): AnimState {
  let facing = a.facing;
  if (velx > 0) facing = 1;
  else if (velx < 0) facing = -1;
  const mask = flying ? 15 : 63;
  const counter = (a.counter + 1) & mask;
  return { counter, facing };
}

/** move.c holds frame 0 while idle (velx === 0), even though girlshp still runs. */
export function frameIndex(a: AnimState, velx: number): number {
  return velx === 0 ? 0 : a.counter >> 2;
}
