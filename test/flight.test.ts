import { describe, expect, it } from "vitest";
import { CANON_CONSTANTS, stepFlight, type FlightBody } from "../src/designer/flight/physics";
import { applyWorld, isOverGap, makeWorld } from "../src/designer/flight/world";
import { frameIndex, initAnim, stepAnim } from "../src/designer/flight/anim";

const body = (over: Partial<FlightBody> = {}): FlightBody => ({
  xp: 0,
  yp: 1000,
  velx: 0,
  vely: 0,
  fuel: 10,
  ...over,
});

describe("stepFlight — vertical (grav_sim)", () => {
  it("thrust and gravity are equal-magnitude opposites (the signature floatiness)", () => {
    const thrusted = stepFlight(body({ fuel: 10 }), { joyx: 0, thrust: true }, CANON_CONSTANTS);
    const fell = stepFlight(body({ fuel: 10 }), { joyx: 0, thrust: false }, CANON_CONSTANTS);
    expect(thrusted.vely).toBe(-1);
    expect(fell.vely).toBe(1);
  });

  it("burns exactly 1 fuel per thrust-frame", () => {
    const b = stepFlight(body({ fuel: 10 }), { joyx: 0, thrust: true }, CANON_CONSTANTS);
    expect(b.fuel).toBe(9);
  });

  it("fuel exhaustion mid-flight: gravity wins even while thrust is held", () => {
    const b = stepFlight(body({ fuel: 0, vely: -10 }), { joyx: 0, thrust: true }, CANON_CONSTANTS);
    expect(b.fuel).toBe(0);
    expect(b.vely).toBe(-9); // gravity (+1), not thrust — no fuel means no lift
  });

  it("clamps vely to ±velyClamp", () => {
    const up = stepFlight(body({ vely: -64, fuel: 999 }), { joyx: 0, thrust: true }, CANON_CONSTANTS);
    expect(up.vely).toBe(-64);
    const down = stepFlight(body({ vely: 64 }), { joyx: 0, thrust: false }, CANON_CONSTANTS);
    expect(down.vely).toBe(64);
  });

  it("add_check_y ceiling: yp clamps to 0 and kills vely", () => {
    const b = stepFlight(body({ yp: 0, vely: -5, fuel: 999 }), { joyx: 0, thrust: true }, CANON_CONSTANTS);
    expect(b.yp).toBe(0);
    expect(b.vely).toBe(0);
  });
});

describe("stepFlight — horizontal (add_x)", () => {
  it("accelerates toward joyx", () => {
    const b = stepFlight(body(), { joyx: 1, thrust: false }, CANON_CONSTANTS);
    expect(b.velx).toBe(1);
  });

  it("decays to 0 by 1/frame when joyx is released", () => {
    const b = stepFlight(body({ velx: 3 }), { joyx: 0, thrust: false }, CANON_CONSTANTS);
    expect(b.velx).toBe(2);
  });

  it("snappy turnaround: extra decel when reversing direction", () => {
    const b = stepFlight(body({ velx: 5 }), { joyx: -1, thrust: false }, CANON_CONSTANTS);
    // velx += joyx (5 -> 4), then extra -1 for reversing while velx>0 => 3
    expect(b.velx).toBe(3);
  });

  it("clamps velx to ±velxClamp", () => {
    const b = stepFlight(body({ velx: 32 }), { joyx: 1, thrust: false }, CANON_CONSTANTS);
    expect(b.velx).toBe(32);
  });

  it("tuning panel constants apply immediately (runtime copy, not canon)", () => {
    const tuned = { ...CANON_CONSTANTS, friction: 3 };
    const b = stepFlight(body({ velx: 5 }), { joyx: 0, thrust: false }, tuned);
    expect(b.velx).toBe(2);
    expect(CANON_CONSTANTS.friction).toBe(1); // canon untouched
  });
});

describe("world — floor, gap, respawn", () => {
  it("lands on solid floor and zeroes vely", () => {
    const w = makeWorld();
    const b = applyWorld({ xp: 0, yp: w.floorYp + 5, velx: 0, vely: 3, fuel: 10 }, w);
    expect(b.yp).toBe(w.floorYp);
    expect(b.vely).toBe(0);
  });

  it("does not land over the gap", () => {
    const w = makeWorld();
    const midGap = (w.gapStartXp + w.gapEndXp) / 2;
    const b = applyWorld({ xp: midGap, yp: w.floorYp + 5, velx: 0, vely: 3, fuel: 10 }, w);
    expect(b.yp).toBe(w.floorYp + 5); // kept falling, no floor here
  });

  it("respawns at the spawn point once fallen past the gap's fall limit", () => {
    const w = makeWorld();
    const midGap = (w.gapStartXp + w.gapEndXp) / 2;
    const b = applyWorld({ xp: midGap, yp: w.fallLimitYp, velx: 12, vely: 20, fuel: 7 }, w);
    expect(b.xp).toBe(w.spawnXp);
    expect(b.yp).toBe(w.spawnYp);
    expect(b.velx).toBe(0);
    expect(b.vely).toBe(0);
    expect(b.fuel).toBe(7); // fuel is not refilled by a respawn
  });

  it("isOverGap is true only within [gapStartXp, gapEndXp]", () => {
    const w = makeWorld();
    expect(isOverGap(w.gapStartXp - 1, w)).toBe(false);
    expect(isOverGap(w.gapStartXp, w)).toBe(true);
    expect(isOverGap(w.gapEndXp, w)).toBe(true);
    expect(isOverGap(w.gapEndXp + 1, w)).toBe(false);
  });

  it("clamps xp to the sandbox's horizontal bounds", () => {
    const w = makeWorld();
    const b = applyWorld({ xp: w.maxXp + 500, yp: 0, velx: 5, vely: 0, fuel: 10 }, w);
    expect(b.xp).toBe(w.maxXp);
  });
});

describe("gap crossing — integration (physics + world over many ticks)", () => {
  function run(thrustOverGap: boolean): "crossed" | "fell" {
    const w = makeWorld();
    let b: FlightBody = { xp: w.gapStartXp - 20 * 32, yp: w.floorYp, velx: 32, vely: 0, fuel: 999 };
    for (let i = 0; i < 300; i++) {
      const over = isOverGap(b.xp, w);
      b = stepFlight(b, { joyx: 1, thrust: thrustOverGap && over }, CANON_CONSTANTS);
      b = applyWorld(b, w);
      if (b.xp === w.spawnXp && b.yp === w.spawnYp) return "fell";
      if (b.xp > w.gapEndXp && b.yp <= w.floorYp) return "crossed";
    }
    throw new Error("simulation did not resolve");
  }

  it("the gap is uncrossable without jetpack", () => {
    expect(run(false)).toBe("fell");
  });

  it("the gap is crossable with jetpack", () => {
    expect(run(true)).toBe("crossed");
  });
});

describe("anim — set_bob() ground/air cadence", () => {
  it("tracks facing from velx sign", () => {
    let a = initAnim();
    a = stepAnim(a, -5, false);
    expect(a.facing).toBe(-1);
    a = stepAnim(a, 5, false);
    expect(a.facing).toBe(1);
  });

  it("holds frame 0 while idle even as the counter keeps running", () => {
    const a = stepAnim(initAnim(), 0, false);
    expect(frameIndex(a, 0)).toBe(0);
  });

  it("wraps the ground bank's 16-frame index (counter &63, >>2)", () => {
    let a = initAnim();
    for (let i = 0; i < 64; i++) a = stepAnim(a, 1, false);
    expect(a.counter).toBe(0);
    expect(frameIndex(a, 1)).toBe(0);
  });

  it("wraps the air bank's 4-frame index (counter &15, >>2)", () => {
    let a = initAnim();
    for (let i = 0; i < 16; i++) a = stepAnim(a, 1, true);
    expect(a.counter).toBe(0);
    expect(frameIndex(a, 1)).toBe(0);
  });
});
