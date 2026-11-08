import { describe, expect, it } from 'vitest';
import { MATRIX_COLS, MATRIX_ROWS, type DeckMatrix } from '../levels/decoder';
import { APPEAR_SHP, DEFAULT_FLAGS, MGROUND, type PlayInput, newGame, tickLaira } from './move';
import { type Fixture, decodeInputs, encodeInputs, endOf, parseQuery, replay, toQuery } from './trainer';

const cells = new Uint16Array(MATRIX_ROWS * MATRIX_COLS);
for (let c = 0; c < MATRIX_COLS; c++) cells[7 * MATRIX_COLS + c] = MGROUND;
const floor: DeckMatrix = { rows: MATRIX_ROWS, cols: MATRIX_COLS, cells };

describe('URL state', () => {
  it('?noclip=1&fuel=0 sets both without clicking', () => {
    const { flags, any } = parseQuery('?noclip=1&fuel=0');
    expect(flags.noclip).toBe(true);
    expect(flags.fuelBurn).toBe(false);
    expect(any).toBe(true);
  });

  it('round-trips switches, devices and probes; canon is a clean URL', () => {
    expect(toQuery(DEFAULT_FLAGS, { tiles: false, box: false })).toBe('');
    const flags = { ...DEFAULT_FLAGS, gravity: false, mines: false, devices: 0x80000005 };
    const q = toQuery(flags, { tiles: true, box: false });
    const back = parseQuery('?' + q);
    expect(back.flags).toEqual(flags);
    expect(back.probes).toEqual({ tiles: true, box: false });
  });

  it('god=1 clears every kill cause and fuel burn, not gravity', () => {
    const { flags } = parseQuery('?god=1');
    expect([flags.fuelBurn, flags.energyKills, flags.trapKills, flags.fallKills, flags.foeKills]).toEqual([false, false, false, false, false]);
    expect(flags.gravity).toBe(true);
  });
});

describe('input recorder', () => {
  it('run-length encoding round-trips', () => {
    const s: PlayInput[] = [
      { joyx: 1, thrust: false }, { joyx: 1, thrust: false }, { joyx: 0, thrust: true }, { joyx: -1, thrust: true },
    ];
    expect(encodeInputs(s)).toEqual([[1, 0, 2], [0, 1, 1], [-1, 1, 1]]);
    expect(decodeInputs(encodeInputs(s))).toEqual(s);
  });

  it('a recorded stream replays to the identical landing cell', () => {
    const flags = { ...DEFAULT_FLAGS, fuelBurn: false };
    const start = tickN(newGame(0), APPEAR_SHP + 1);
    const inputs: PlayInput[] = [
      ...Array(30).fill({ joyx: 1, thrust: false }),
      ...Array(40).fill({ joyx: 1, thrust: true }),
      ...Array(120).fill({ joyx: 0, thrust: false }),
    ];
    let l = start;
    for (const i of inputs) l = tickLaira(l, floor, i, flags);
    const f: Fixture = { flags, start, inputs: encodeInputs(inputs), end: endOf(l) };
    expect(f.end.row).toBe(7); // came down on the floor row
    expect(replay(f, floor)).toEqual(f.end);
    expect(replay(f, floor)).toEqual(replay(f, floor));
  });
});

function tickN(l: ReturnType<typeof newGame>, n: number) {
  for (let t = 0; t < n; t++) l = tickLaira(l, floor, { joyx: 0, thrust: false }, DEFAULT_FLAGS);
  return l;
}
