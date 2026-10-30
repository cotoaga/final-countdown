import { describe, expect, it } from 'vitest';
import { MATRIX_COLS, MATRIX_ROWS, type DeckMatrix } from '../levels/decoder';
import {
  APPEAR_SHP,
  DEATH_TICKS,
  DEFAULT_FLAGS,
  FALL_YP,
  MENERGY,
  MGROUND,
  type Laira,
  type PlayFlags,
  type PlayInput,
  mxpOf,
  newGame,
  spawn,
  tickLaira,
} from './move';

const IDLE: PlayInput = { joyx: 0, thrust: false };
const RIGHT: PlayInput = { joyx: 1, thrust: false };
const UP: PlayInput = { joyx: 0, thrust: true };

function matrix(paint: (set: (row: number, col: number, bits: number) => void) => void): DeckMatrix {
  const cells = new Uint16Array(MATRIX_ROWS * MATRIX_COLS);
  paint((row, col, bits) => {
    cells[row * MATRIX_COLS + col] |= bits;
  });
  return { rows: MATRIX_ROWS, cols: MATRIX_COLS, cells };
}

/** Floor across row 7 — matches startdef[0]: yp row 4 offset 11, feet on row 7. */
const FLOOR_ROW = 7;
const flat = matrix((set) => {
  for (let c = 0; c < MATRIX_COLS; c++) set(FLOOR_ROW, c, MGROUND);
});

function run(l: Laira, m: DeckMatrix, input: PlayInput, ticks: number, flags: PlayFlags = DEFAULT_FLAGS): Laira {
  for (let t = 0; t < ticks; t++) l = tickLaira(l, m, input, flags);
  return l;
}

/** Skip SMAPP: 83 ticks of appear, stick ignored. */
const alive = (l: Laira, m: DeckMatrix) => run(l, m, IDLE, APPEAR_SHP + 1);

describe('spawn — init_move / startdef', () => {
  it('deck 1 spawn: xpos 720, Laira at world col 53, yp row 4 offset 11, appearing', () => {
    const l = newGame(0);
    expect(l.spos >> 5).toBe(720);
    expect(mxpOf(l) >> 9).toBe(53);
    expect(l.yp >> 10).toBe(4);
    expect((l.yp >> 6) & 15).toBe(11);
    expect(l.phase).toBe('appear');
    expect(l.fuel).toBe(0);
  });

  it('appear holds position (no gravity, stick ignored) and ends alive', () => {
    const l0 = newGame(0);
    const l = run(l0, flat, RIGHT, APPEAR_SHP + 1);
    expect(l.phase).toBe('alive');
    expect(l.yp).toBe(l0.yp);
    expect(l.velx).toBe(0);
  });
});

describe('check_ground — landing only at offset 11 on an MGROUND feet cell', () => {
  it('stands still on a flat floor', () => {
    const l = run(alive(newGame(0), flat), flat, IDLE, 100);
    expect(l.flying).toBe(0);
    expect(l.vely).toBe(0);
    expect((l.yp >> 6) & 15).toBe(11);
  });

  it('falls through a gap and dies at the 180 px floor', () => {
    const gap = matrix((set) => {
      for (let c = 0; c < MATRIX_COLS; c++) if (c < 60 || c > 62) set(FLOOR_ROW, c, MGROUND);
    });
    // Park her over col 61: mxp = spos + xp, xp = 130*32.
    const over = { ...newGame(0), spos: 61 * 512 - 130 * 32, phase: 'alive' as const, shp: 0 };
    let l = run(over, gap, IDLE, 10);
    expect(l.flying).toBe(1);
    expect(l.vely).toBeGreaterThan(0);
    // 6720 units to the floor at ≤64/tick ≈ 110 ticks; well inside the 200-tick death counter.
    l = run(l, gap, IDLE, 150);
    expect(l.yp).toBeLessThanOrEqual(FALL_YP);
    expect(l.phase).toBe('die');
    expect(l.visible).toBe(false);
  });

  it('fall death respawns at startdef after 200 ticks with fuelval fuel', () => {
    const empty = matrix(() => {});
    let l = run({ ...newGame(0), phase: 'alive', shp: 0 }, empty, IDLE, 150);
    expect(l.phase).toBe('die');
    const diedAt = l.tick;
    while (l.phase === 'die') l = tickLaira(l, empty, IDLE, DEFAULT_FLAGS);
    expect(l.phase).toBe('appear');
    expect(l.tick - diedAt).toBeLessThanOrEqual(DEATH_TICKS);
    expect(mxpOf(l) >> 9).toBe(53);
    expect(l.fueladd).toBe(150); // fuelval[0][0]
  });

  it('fallKills=false clamps at the floor without dying', () => {
    const empty = matrix(() => {});
    const l = run({ ...newGame(0), phase: 'alive', shp: 0 }, empty, IDLE, 400, { ...DEFAULT_FLAGS, fallKills: false });
    expect(l.yp).toBe(FALL_YP);
    expect(l.phase).toBe('alive');
  });

  it('ceiling: rising stops at offset 15 under an MGROUND head cell', () => {
    const roof = matrix((set) => {
      for (let c = 0; c < MATRIX_COLS; c++) {
        set(FLOOR_ROW, c, MGROUND);
        set(3, c, MGROUND);
      }
    });
    let l = alive(newGame(0), roof);
    l = { ...l, fuel: 1000, fueladd: 0 };
    let minYp = l.yp;
    for (let t = 0; t < 200; t++) {
      l = tickLaira(l, roof, UP, DEFAULT_FLAGS);
      minYp = Math.min(minYp, l.yp);
    }
    expect(minYp).toBeGreaterThanOrEqual(3 * 1024 + 15 * 64);
  });
});

describe('check_walls — right wall at col+3, left wall at col-1', () => {
  const walled = matrix((set) => {
    for (let c = 0; c < MATRIX_COLS; c++) set(FLOOR_ROW, c, MGROUND);
    for (let r = 4; r < FLOOR_ROW; r++) set(r, 58, MGROUND);
  });

  it('running right stops before the wall column', () => {
    const l = run(alive(newGame(0), walled), walled, RIGHT, 400);
    expect((mxpOf(l) >> 9) + 3).toBeLessThanOrEqual(58);
    expect(Math.abs(l.velx)).toBeLessThanOrEqual(1);
  });

  it('noclip (+ gravity off, or she just falls forever) walks straight through', () => {
    const ghost = { ...DEFAULT_FLAGS, noclip: true, gravity: false };
    const l = run(alive(newGame(0), walled), walled, RIGHT, 400, ghost);
    expect(mxpOf(l) >> 9).toBeGreaterThan(60);
    expect(l.phase).toBe('alive');
  });
});

describe('grav_sim + no_name — fuel', () => {
  it('fills +2/tick from fueladd 200 to 400, then burns 1/tick on thrust', () => {
    let l = alive(newGame(0), flat);
    l = run(l, flat, IDLE, 200);
    expect(l.fuel).toBe(400);
    expect(l.fueladd).toBe(0);
    const before = l.fuel;
    l = run(l, flat, UP, 10);
    expect(l.fuel).toBe(before - 10);
    expect(l.flying).toBe(1);
  });

  it('fuelBurn=false thrusts for free', () => {
    let l = run(alive(newGame(0), flat), flat, IDLE, 200);
    l = run(l, flat, UP, 50, { ...DEFAULT_FLAGS, fuelBurn: false });
    expect(l.fuel).toBe(400);
  });
});

describe('kills — MENERGY body cell', () => {
  // Field two columns right of spawn (body cell row 6 at spawn is col 54).
  const field = matrix((set) => {
    for (let c = 0; c < MATRIX_COLS; c++) set(FLOOR_ROW, c, MGROUND);
    for (let c = 56; c < 60; c++) set(6, c, MENERGY);
  });

  it('touching a field arms collision=1; the death fires once velx == 0 (get_new_anim), then respawns clean', () => {
    let l = alive(spawn(0, 0, 0), field);
    // Run into it, keep running: hit registers but she does not die while moving.
    l = run(l, field, RIGHT, 60);
    expect(l.collision).toBe(1);
    expect(l.phase).toBe('alive');
    // Release the stick: brake to zero, then SMDIE.
    l = run(l, field, IDLE, 60);
    expect(l.phase).toBe('die');
    expect(l.collision).toBeGreaterThanOrEqual(2);
    while (l.phase === 'die') l = tickLaira(l, field, IDLE, DEFAULT_FLAGS);
    expect(l.phase).toBe('appear');
    expect(l.collision).toBe(0);
  });

  it('energyKills=false ignores the field', () => {
    let l = alive(spawn(0, 0, 0), field);
    l = run(l, field, RIGHT, 60, { ...DEFAULT_FLAGS, energyKills: false });
    l = run(l, field, IDLE, 60, { ...DEFAULT_FLAGS, energyKills: false });
    expect(l.collision).toBe(0);
    expect(l.phase).toBe('alive');
  });
});

describe('scroll — camera margins and recentre', () => {
  it('xp stays inside 80..180 px while running right; spos advances', () => {
    let l = alive(newGame(0), flat);
    const spos0 = l.spos;
    l = run(l, flat, RIGHT, 300);
    expect(l.xp >> 5).toBeLessThanOrEqual(181);
    expect(l.spos).toBeGreaterThan(spos0);
    expect(mxpOf(l)).toBe(l.spos + l.xp);
  });
});
