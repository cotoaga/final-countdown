/**
 * Ground movement state machine — verbatim port of the specmv ground subset
 * from reference/src/move.c.
 *
 * Transitions (move.c:410-431, evaluated only on the ground):
 *   !velx && joyx == +1          -> SMSTARTR  (girlshp = 0)
 *   !velx && joyx == -1          -> SMSTARTL  (girlshp = 0)
 *   velx >= +15 && joyx != +1    -> SMSTOPR   (girlshp = 15)
 *   velx <= -15 && joyx != -1    -> SMSTOPL   (girlshp = 15)
 *
 * State bodies (move.c:82-110):
 *   SMSTART*: forces joyx toward the run, locks joyy; ++girlshp, exit > 15
 *   SMSTOP*:  locks joyx = joyy = 0 (stick ignored, friction brakes);
 *             --girlshp floors at 0, exit only when velx == 0
 *
 * Sprites (move.c set_bob:484-488): girlgsr/gsl[girlshp >> 1] — one 8-frame
 * lean bank per side, played forward on start, backward on stop.
 *
 * The original's "turn-around" is EXACTLY this chain: running right + stick
 * left => SMSTOPR (brake, controls locked) -> velx decays to 0 -> SMGROUND ->
 * SMSTARTL (lean into the opposite run). There is NO dedicated bank for
 * direction reversal; girltr[] is the use-device/hide/lift rotation
 * (move.c:361 — stick UP at an MCOMPUTER/MSWITCH tile), out of this fence.
 *
 * Below the SMSTOP threshold (|velx| < 15, move.c:424) the original flips
 * direction with no ceremony — faithfully preserved here.
 */

export const STOP_THRESHOLD = 15; // move.c:424

export type GroundStateName = "ground" | "start-r" | "start-l" | "stop-r" | "stop-l";

export interface GroundState {
  name: GroundStateName;
  shp: number; // girlshp
}

export interface EffectiveInput {
  joyx: -1 | 0 | 1;
  thrust: boolean;
}

export function initGroundState(): GroundState {
  return { name: "ground", shp: 0 };
}

/**
 * One 50 Hz tick of the specmv ground subset. Pure. Call BEFORE the physics
 * step and feed the returned effective input into stepFlight — start/stop
 * states override the player's stick exactly like move.c does.
 * Airborne resets to "ground": the original's air start/end states are
 * separate banks (girlasr/asl) and outside this sandbox's fence.
 */
export function stepGroundState(
  s: GroundState,
  velx: number,
  joyx: -1 | 0 | 1,
  thrust: boolean,
  onGround: boolean,
): { next: GroundState; input: EffectiveInput } {
  if (!onGround) {
    return { next: { name: "ground", shp: 0 }, input: { joyx, thrust } };
  }
  switch (s.name) {
    case "start-r":
    case "start-l": {
      const dir: -1 | 1 = s.name === "start-r" ? 1 : -1;
      const shp = s.shp + 1;
      if (shp > 15) {
        return { next: { name: "ground", shp: 0 }, input: { joyx, thrust } };
      }
      // contstartground: joyx forced toward the run, joyy locked.
      return { next: { name: s.name, shp }, input: { joyx: dir, thrust: false } };
    }
    case "stop-r":
    case "stop-l": {
      if (s.shp === 0 && velx === 0) {
        return { next: { name: "ground", shp: 0 }, input: { joyx: 0, thrust: false } };
      }
      const shp = Math.max(0, s.shp - 1);
      // SMSTOP*: joyx = joyy = 0 — Laira brakes, the stick is ignored.
      return { next: { name: s.name, shp }, input: { joyx: 0, thrust: false } };
    }
    case "ground": {
      if (velx === 0 && joyx === 1) {
        return { next: { name: "start-r", shp: 0 }, input: { joyx: 1, thrust: false } };
      }
      if (velx === 0 && joyx === -1) {
        return { next: { name: "start-l", shp: 0 }, input: { joyx: -1, thrust: false } };
      }
      if (velx >= STOP_THRESHOLD && joyx !== 1) {
        return { next: { name: "stop-r", shp: 15 }, input: { joyx: 0, thrust: false } };
      }
      if (velx <= -STOP_THRESHOLD && joyx !== -1) {
        return { next: { name: "stop-l", shp: 15 }, input: { joyx: 0, thrust: false } };
      }
      return { next: s, input: { joyx, thrust } };
    }
  }
}

/** Sprite override: null = normal walk/idle logic; else girlgs bank + frame. */
export function groundStateSprite(s: GroundState): { side: "r" | "l"; frame: number } | null {
  switch (s.name) {
    case "start-r":
    case "stop-r":
      return { side: "r", frame: s.shp >> 1 };
    case "start-l":
    case "stop-l":
      return { side: "l", frame: s.shp >> 1 };
    default:
      return null;
  }
}
