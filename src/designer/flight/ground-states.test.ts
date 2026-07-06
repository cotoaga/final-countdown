import { describe, expect, it } from 'vitest';
import { CANON_CONSTANTS, stepFlight, type FlightBody } from './physics';
import {
  STOP_THRESHOLD,
  groundStateSprite,
  initGroundState,
  stepGroundState,
  type GroundState,
} from './ground-states';

describe('ground state machine — specmv port', () => {
  it('start: 16 forced ticks of lean, then back to ground', () => {
    let s = initGroundState();
    let r = stepGroundState(s, 0, 1, false, true);
    expect(r.next.name).toBe('start-r');
    expect(r.input.joyx).toBe(1);

    s = r.next;
    for (let t = 0; t < 15; t++) {
      r = stepGroundState(s, 5, 0, true, true); // player releases; state overrides
      expect(r.next.name).toBe('start-r');
      expect(r.input.joyx).toBe(1); // forced toward the run
      expect(r.input.thrust).toBe(false); // joyy locked
      s = r.next;
    }
    r = stepGroundState(s, 16, 0, false, true);
    expect(r.next.name).toBe('ground'); // shp > 15 → exit
  });

  it('stop triggers at |velx| >= 15 with stick released or opposed', () => {
    const g = initGroundState();
    expect(stepGroundState(g, STOP_THRESHOLD, 0, false, true).next.name).toBe('stop-r');
    expect(stepGroundState(g, STOP_THRESHOLD, -1, false, true).next.name).toBe('stop-r');
    expect(stepGroundState(g, STOP_THRESHOLD - 1, 0, false, true).next.name).toBe('ground');
    expect(stepGroundState(g, -STOP_THRESHOLD, 1, false, true).next.name).toBe('stop-l');
  });

  it('stop locks the stick and holds until velx reaches 0', () => {
    let s: GroundState = { name: 'stop-r', shp: 0 }; // frames exhausted
    const r = stepGroundState(s, 3, -1, true, true); // still sliding
    expect(r.next.name).toBe('stop-r');
    expect(r.input.joyx).toBe(0);
    expect(r.input.thrust).toBe(false);
    const done = stepGroundState(r.next, 0, -1, false, true);
    expect(done.next.name).toBe('ground');
  });

  it('THE REMEMBERED TURN: run right, flip stick — stop-r brakes to 0, start-l leans left', () => {
    let body: FlightBody = { xp: 1000, yp: 0, velx: 0, vely: 0, fuel: 100 };
    let state = initGroundState();
    const seen: string[] = [];
    const drive = (joyx: -1 | 0 | 1, until: (b: FlightBody, s: GroundState) => boolean) => {
      for (let t = 0; t < 400; t++) {
        const g = stepGroundState(state, body.velx, joyx, false, true);
        state = g.next;
        if (seen[seen.length - 1] !== state.name) seen.push(state.name);
        body = stepFlight(body, { joyx: g.input.joyx, thrust: g.input.thrust }, CANON_CONSTANTS);
        body = { ...body, yp: 0, vely: 0 }; // pin to ground for this test
        if (until(body, state)) return;
      }
      throw new Error('drive() never reached condition');
    };

    drive(1, (b) => b.velx >= STOP_THRESHOLD + 5); // run right past threshold
    drive(-1, (_b, s) => s.name === 'stop-r'); // flip the stick → brake
    drive(-1, (b, s) => s.name === 'ground' && b.velx === 0); // brake completes
    drive(-1, (_b, s) => s.name === 'start-l'); // lean into the left run
    drive(-1, (b) => b.velx <= -5); // and she is running left

    expect(seen).toContain('start-r');
    expect(seen).toContain('stop-r');
    expect(seen).toContain('start-l');
    expect(seen.indexOf('stop-r')).toBeGreaterThan(seen.indexOf('start-r'));
    expect(seen.indexOf('start-l')).toBeGreaterThan(seen.indexOf('stop-r'));
  });

  it('airborne resets to ground and passes input through', () => {
    const s: GroundState = { name: 'stop-r', shp: 7 };
    const r = stepGroundState(s, 20, 1, true, false);
    expect(r.next.name).toBe('ground');
    expect(r.input.joyx).toBe(1);
    expect(r.input.thrust).toBe(true);
  });

  it('sprite override maps state to girlgs bank side + frame (shp>>1)', () => {
    expect(groundStateSprite({ name: 'start-r', shp: 15 })).toEqual({ side: 'r', frame: 7 });
    expect(groundStateSprite({ name: 'stop-l', shp: 4 })).toEqual({ side: 'l', frame: 2 });
    expect(groundStateSprite({ name: 'ground', shp: 0 })).toBeNull();
  });
});
