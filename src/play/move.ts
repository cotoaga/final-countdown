/**
 * Laira on real deck geometry — port of move_own() (reference/src/move.c)
 * with the tile collision from reference/src/mv.asm (check_walls,
 * check_ground), the fuel fill from devices.c no_name(), and the death /
 * respawn counter from main.c's inner loop. Pure: no DOM, no globals.
 *
 * Units (move.c): mxp = world x in 1/32 px, yp = sprite TOP in 1/64 px,
 * velx/vely in the same units. Matrix cell = matrix[yp>>10][mxp>>9]
 * (mv.asm: asr #10 / asr #9). Laira's body column is col+1; walls are
 * probed at col+3 (right) and col-1 (left); feet are row+3.
 *
 * Tick order is move_own() verbatim (move.c:49-60):
 *   no_name → anim_joystick → check_walls → get_new_anim → grav_sim →
 *   check_ground → add_x → add_check_y → scroll → set_bob
 *
 * `flags` is the trainer hook (COT-253). The original's own trainer flag is
 * `master` (main.c:286, move.c grav_sim/add_check_y/get_new_anim): it skips
 * fuel burn and every kill. We split it per cause so a surprise death can
 * be attributed to one bit.
 */
import { type DeckMatrix, matrixCell } from '../levels/decoder';
import { type AnimState, initAnim, stepAnim } from '../designer/flight/anim';
import { type GroundState, initGroundState, stepGroundState } from '../designer/flight/ground-states';
import { CANON_CONSTANTS } from '../designer/flight/physics';

// externals.c:38-47 — matrix cell bits.
export const MGROUND = 0x8000;
export const MENERGY = 0x4000;
export const MTRAP = 0x1000;
export const CHECKTHING = 0x3800;

// move.c add_check_y: yp > 180*64 is the fall-death floor.
export const FALL_YP = 180 * 64;
// main.c:302 — collision counts from 2 up to 200, then respawn.
export const DEATH_TICKS = 200;
// move.c init_move: girlshp = 82, specmv = SMAPP — appear plays girle backwards.
export const APPEAR_SHP = 82;
// move.c set_bob SMDIE: girlshp caps at 81 (21 frames @ >>2).
export const DIE_SHP_MAX = 81;
// devices.c no_name: fuel caps at 1000, fills +2 per tick while fueladd > 0.
export const FUEL_MAX = 1000;

// start.c startdef[MAXLEVELS] — { x[2], y[2] }: x = scroll xpos in px,
// y = yp in 1/64 px. Index by exno (0 = arrived from below, 1 = from above).
export const STARTDEF: Array<{ x: [number, number]; y: [number, number] }> = [
  { x: [45 * 16, 80 * 16], y: [4 * 16 * 64 + 704, 4 * 16 * 64 + 704] },
  { x: [54 * 16, 25 * 16], y: [8 * 16 * 64 + 704, 8 * 16 * 64 + 704] },
  { x: [0 * 16, 0 * 16], y: [0 * 16 * 64 + 704, 4 * 16 * 64 + 704] },
  { x: [99 * 16, 70 * 16], y: [0 * 16 * 64 + 704, 0 * 16 * 64 + 704] },
  { x: [66 * 16, 9 * 16], y: [4 * 16 * 64 + 704, 0 * 16 * 64 + 704] },
  { x: [61 * 16, 61 * 16], y: [0 * 16 * 64 + 704, 0 * 16 * 64 + 704] },
  { x: [0 * 16, 80 * 16], y: [4 * 16 * 64 + 704, 4 * 16 * 64 + 704] },
  { x: [63 * 16, 63 * 16], y: [4 * 16 * 64 + 704, 4 * 16 * 64 + 704] },
];
// main.c fuelval[MAXLEVELS][2] — fueladd after a death, by level and exno.
export const FUELVAL: Array<[number, number]> = [
  [150, 100], [200, 100], [150, 100], [100, 100], [200, 100], [100, 100], [100, 100], [100, 100],
];
// main.c:268 — first entry into the game: fuel 0, fueladd 200.
export const INITIAL_FUELADD = 200;

export interface PlayFlags {
  fuelBurn: boolean; // grav_sim: `if (!master) --fuel`
  energyKills: boolean; // mv.asm .15: MENERGY → collision = 1
  trapKills: boolean; // mv.asm .13: MTRAP under feet, not flying → collision = 1
  fallKills: boolean; // add_check_y: yp > 180*64 → collision = 2
  gravity: boolean; // grav_sim: vely += 1
  noclip: boolean; // MGROUND ignored by check_walls / check_ground
}

export const DEFAULT_FLAGS: PlayFlags = {
  fuelBurn: true,
  energyKills: true,
  trapKills: true,
  fallKills: true,
  gravity: true,
  noclip: false,
};

export type Phase = 'appear' | 'alive' | 'die';

export interface Laira {
  level: number;
  exno: 0 | 1;
  spos: number; // scroll position, 1/32 px (move.c spos); xpos = spos >> 5
  xp: number; // screen x of the bob, 1/32 px
  yp: number; // sprite top, 1/64 px
  velx: number;
  vely: number;
  fuel: number;
  fueladd: number;
  flying: 0 | 1;
  collision: number; // 0 ok · 1 hit (dies once velx == 0) · ≥2 dying counter
  phase: Phase;
  shp: number; // girlshp for appear/die
  ground: GroundState;
  anim: AnimState;
  tick: number;
  visible: boolean; // bobon
}

export interface PlayInput {
  joyx: -1 | 0 | 1;
  thrust: boolean; // joyy == -1
}

/** move.c mxp — world x. Invariant: mxp == spos + xp (init_move keeps it). */
export function mxpOf(l: Laira): number {
  return l.spos + l.xp;
}

function cell(m: DeckMatrix, row: number, col: number): number {
  if (row < 0 || row >= m.rows || col < 0 || col >= m.cols) return 0;
  return matrixCell(m, row, col);
}

/** move.c init_move(level) — also main.c's respawn path (leveladd == 0). */
export function spawn(level: number, exno: 0 | 1, fueladd: number): Laira {
  const sd = STARTDEF[level];
  const xpos = sd.x[exno];
  return {
    level,
    exno,
    spos: xpos * 32,
    xp: 130 * 32,
    yp: sd.y[exno],
    velx: 0,
    vely: 0,
    fuel: 0,
    fueladd,
    flying: 0,
    collision: 0,
    phase: 'appear',
    shp: APPEAR_SHP,
    ground: initGroundState(),
    anim: initAnim(),
    tick: 0,
    visible: true,
  };
}

export function newGame(level = 0): Laira {
  return spawn(level, 0, INITIAL_FUELADD);
}

/**
 * One 50 Hz tick. Returns the next Laira; respawn is folded in when the
 * death counter expires (main.c:303-325, minus the itime bookkeeping that
 * belongs to the M2 sim).
 */
export function tickLaira(l: Laira, m: DeckMatrix, raw: PlayInput, flags: PlayFlags): Laira {
  let { spos, xp, yp, velx, vely, fuel, fueladd, flying, collision, phase, shp, ground, anim, visible } = l;
  const tick = l.tick + 1;

  // main.c:303-325 — dying counter; respawn on the same deck when it expires.
  if (collision > 1) {
    collision++;
    if (collision >= DEATH_TICKS) {
      const next = spawn(l.level, l.exno, FUELVAL[l.level][l.exno]);
      return { ...next, tick };
    }
  }

  // ── no_name() (devices.c) — fuel fill from fueladd.
  if (fueladd) {
    if (fuel < FUEL_MAX) {
      fueladd--;
      fuel += 2;
    } else fueladd = 0;
  }

  // ── anim_joystick() — SMAPP / SMDIE own the stick; otherwise the
  // start/stop/air-lean subset (ground-states.ts) may override it.
  let joyx: -1 | 0 | 1 = raw.joyx;
  let thrust = raw.thrust;
  if (phase === 'appear') {
    // move.c SMAPP: joyx = joyy = 0 until --girlshp < 0 → SMGROUND.
    joyx = 0;
    thrust = false;
    shp--;
    if (shp < 0) {
      shp = 0;
      phase = 'alive';
    }
  } else if (phase === 'die') {
    joyx = 0;
    thrust = false;
  }

  // ── check_walls() (mv.asm). Asm order is anim_joystick (state bodies
  // force joyx) → check_walls (clear joyx, nudge velx) → get_new_anim
  // (transitions). ground-states.ts fuses bodies + transitions, so: the
  // push direction is what anim_joystick would have produced (stick, or the
  // lean state's forced direction); walls clear it; transitions see the
  // cleared stick. Nudge only in SMFLY/SMGROUND (pre-transition specmv).
  const row = yp >> 10;
  const col = mxpOf(l) >> 9;
  const solid = (r: number, c: number) => !flags.noclip && (cell(m, r, c) & MGROUND) !== 0;
  const wallR = solid(row + 2, col + 3);
  const wallL = solid(row + 2, col - 1) || solid(row + 1, col - 1);
  const push: -1 | 0 | 1 =
    ground.name === 'start-r' || ground.name === 'sair-r'
      ? 1
      : ground.name === 'start-l' || ground.name === 'sair-l'
        ? -1
        : ground.name === 'ground'
          ? joyx
          : 0;
  if (ground.name === 'ground' && phase === 'alive') {
    if (push > 0 && wallR) velx -= 1; // rmdchk
    if (push < 0 && wallL) velx += 1; // lmdchk
  }
  if (joyx > 0 && wallR) joyx = 0;
  if (joyx < 0 && wallL) joyx = 0;

  let eff: PlayInput = { joyx, thrust };
  if (phase === 'alive') {
    const g = stepGroundState(ground, velx, joyx, thrust, flying === 0);
    ground = g.next;
    eff = { joyx: g.input.joyx, thrust: g.input.thrust };
    // State-forced joyx (SMSTART*/SMSAIR*) meets the wall like the stick does.
    if (eff.joyx > 0 && wallR) eff = { ...eff, joyx: 0 };
    if (eff.joyx < 0 && wallL) eff = { ...eff, joyx: 0 };
  }

  // colchk: body cells (row+1, row+2) at col+1 inside ground → push down 2px.
  // (Lift states exempt in the asm; no lifts in this fence.)
  if (solid(row + 1, col + 1)) yp += 128;
  if (solid(row + 2, col + 1)) yp += 128;
  // .15: MENERGY in the body cell → hit.
  if (flags.energyKills && (cell(m, row + 2, col + 1) & MENERGY) && collision === 0) collision = 1;
  // .13: armed trap under the feet, on the ground → hit.
  if (
    flags.trapKills &&
    (cell(m, row + 3, col + 1) & CHECKTHING) === MTRAP &&
    collision === 0 &&
    flying === 0
  ) {
    collision = 1;
  }

  // ── get_new_anim() — the death trigger: only once she has stopped.
  if (velx === 0 && collision === 1) {
    collision = 2;
    phase = 'die';
    shp = 0;
  }

  // ── grav_sim()
  if (eff.thrust && fuel > 0 && phase === 'alive') {
    vely -= CANON_CONSTANTS.thrust;
    if (flags.fuelBurn) fuel--;
  } else if (vely < CANON_CONSTANTS.velyClamp) {
    // SMAPP: vely += joyy (0) — no gravity while appearing.
    if (phase !== 'appear' && flags.gravity) vely += CANON_CONSTANTS.gravity;
  }
  if (vely > CANON_CONSTANTS.velyClamp) vely = CANON_CONSTANTS.velyClamp;
  else if (vely < -CANON_CONSTANTS.velyClamp) vely = -CANON_CONSTANTS.velyClamp;

  // ── check_ground() (mv.asm) — recomputed on the possibly pushed-down yp.
  {
    const r2 = yp >> 10;
    flying = 1;
    if (solid(r2 + 3, col + 1) && vely > 0 && ((yp >> 6) & 15) === 11) {
      vely = 0;
      flying = 0;
    }
    // upchk: head cell (row) solid while rising → stop at offset 15, else push down.
    if (solid(r2, col + 1) && vely < 0) {
      if (((yp >> 6) & 15) === 15) vely = 0;
      else yp += 128;
    }
  }

  // ── add_x()
  velx += eff.joyx;
  if (!eff.joyx) {
    if (velx > 0) velx -= CANON_CONSTANTS.friction;
    else if (velx < 0) velx += CANON_CONSTANTS.friction;
  } else {
    if (velx > 0 && eff.joyx < 0) velx -= CANON_CONSTANTS.friction;
    else if (velx < 0 && eff.joyx > 0) velx += CANON_CONSTANTS.friction;
  }
  if (velx < -CANON_CONSTANTS.velxClamp) velx = -CANON_CONSTANTS.velxClamp;
  else if (velx > CANON_CONSTANTS.velxClamp) velx = CANON_CONSTANTS.velxClamp;

  // ── add_check_y()
  yp += vely;
  if (yp < 0) {
    yp = 0;
    vely = 0;
  }
  if (yp > FALL_YP) {
    yp = FALL_YP;
    vely = 0;
    if (flags.fallKills && collision === 0) {
      collision = 2; // instant, no animation; bob switched off
      phase = 'die';
      visible = false;
    }
  }

  // ── scroll() — verbatim: the camera follows only past the 80/180 px
  // margins, clamps at xpos 1..1728, and recentres one px/tick when idle.
  {
    const xpos = spos >> 5;
    let scroll = (xp < 80 * 32 && velx < 0) || (xp > 180 * 32 && velx > 0);
    if ((xpos >= 1728 && velx > 0) || (xpos <= 1 && velx < 0)) scroll = false;
    if (scroll) spos += velx;
    else xp += velx;
    if (!velx && !eff.joyx) {
      if (xp > 131 * 32 && xpos < 1728) {
        spos += 32;
        xp -= 32;
      } else if (xp < 129 * 32 && xpos > 1) {
        spos -= 32;
        xp += 32;
      }
    }
  }

  // ── set_bob() — cadence counters. Die: shp climbs to 81 then bob off.
  if (phase === 'die' && visible) {
    shp++;
    if (shp > DIE_SHP_MAX) {
      shp = DIE_SHP_MAX;
      visible = false;
    }
  }
  anim = stepAnim(anim, velx, flying === 1);

  return {
    ...l,
    spos, xp, yp, velx, vely, fuel, fueladd, flying, collision, phase, shp, ground, anim, tick, visible,
  };
}
