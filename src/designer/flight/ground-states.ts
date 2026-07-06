/**
 * Movement state machine — verbatim port of the specmv GROUND + FLY subsets
 * from reference/src/move.c.
 *
 * Ground transitions (move.c:410-431, evaluated on the ground):
 *   !velx && joyx == +1          -> SMSTARTR  (girlshp = 0)
 *   !velx && joyx == -1          -> SMSTARTL  (girlshp = 0)
 *   velx >= +15 && joyx != +1    -> SMSTOPR   (girlshp = 15)
 *   velx <= -15 && joyx != -1    -> SMSTOPL   (girlshp = 15)
 *
 * Air transitions (move.c:281-299, evaluated in flight) — the SAME shape:
 *   !velx && joyx == ±1          -> SMSAIRR/L (girlshp = 0)
 *   |velx| >= 15, stick off/opposed -> SMEAIRR/L (girlshp = 15)
 *
 * State bodies:
 *   SMSTART* (move.c:82-90):  forces joyx toward the run, LOCKS joyy
 *   SMSTOP*  (move.c:103-110): locks joyx = joyy = 0; exits at shp<0 && velx==0
 *   SMSAIR*  (move.c:92-99):  forces joyx — but THRUST STAYS LIVE (no joyy lock;
 *                             the jetpack never locks, only the stick)
 *   SMEAIR*  (move.c:113-121): locks joyx = 0, thrust stays live; exits at
 *                             shp<0 && velx==0
 *
 * Sprites (set_bob:484-495): girlgsr/gsl[shp>>1] on the ground,
 * girlasr/asl[shp>>1] in the air — forward on start, backward on stop.
 *
 * The original's "turn-around" — on the ground AND in flight — is this chain:
 * brake through zero with the stick locked, then lean into the opposite
 * direction. There is NO dedicated reversal bank; girltr[] is the
 * use-device/hide/lift rotation (move.c:361), out of this fence.
 *
 * Below the threshold (|velx| < 15) the original flips instantly — faithful.
 */

export const STOP_THRESHOLD = 15; // move.c:424 (ground) and move.c:290 (air)

export type GroundStateName =
  | "ground"
  | "start-r"
  | "start-l"
  | "stop-r"
  | "stop-l"
  | "sair-r"
  | "sair-l"
  | "eair-r"
  | "eair-l";

const GROUND_FAMILY: GroundStateName[] = ["start-r", "start-l", "stop-r", "stop-l"];
const AIR_FAMILY: GroundStateName[] = ["sair-r", "sair-l", "eair-r", "eair-l"];

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
  // Crossing the ground/air boundary mid-state resets to neutral — the
  // original's SMGROUND<->SMFLY handoff lives in the collision code.
  if (!onGround && GROUND_FAMILY.includes(s.name)) s = { name: "ground", shp: 0 };
  if (onGround && AIR_FAMILY.includes(s.name)) s = { name: "ground", shp: 0 };
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
    case "sair-r":
    case "sair-l": {
      // move.c:92-99 — joyx forced toward flight direction; thrust LIVE.
      const dir: -1 | 1 = s.name === "sair-r" ? 1 : -1;
      const shp = s.shp + 1;
      if (shp > 15) {
        return { next: { name: "ground", shp: 0 }, input: { joyx, thrust } };
      }
      return { next: { name: s.name, shp }, input: { joyx: dir, thrust } };
    }
    case "eair-r":
    case "eair-l": {
      // move.c:113-121 — stick locked, thrust LIVE; exit at shp 0 && velx 0.
      if (s.shp === 0 && velx === 0) {
        return { next: { name: "ground", shp: 0 }, input: { joyx: 0, thrust } };
      }
      const shp = Math.max(0, s.shp - 1);
      return { next: { name: s.name, shp }, input: { joyx: 0, thrust } };
    }
    case "ground": {
      if (onGround) {
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
      } else {
        // SMFLY transitions, move.c:281-299 — thrust stays live in all of them.
        if (velx === 0 && joyx === 1) {
          return { next: { name: "sair-r", shp: 0 }, input: { joyx: 1, thrust } };
        }
        if (velx === 0 && joyx === -1) {
          return { next: { name: "sair-l", shp: 0 }, input: { joyx: -1, thrust } };
        }
        if (velx >= STOP_THRESHOLD && joyx !== 1) {
          return { next: { name: "eair-r", shp: 15 }, input: { joyx: 0, thrust } };
        }
        if (velx <= -STOP_THRESHOLD && joyx !== -1) {
          return { next: { name: "eair-l", shp: 15 }, input: { joyx: 0, thrust } };
        }
      }
      return { next: s, input: { joyx, thrust } };
    }
  }
}

/** Sprite override: null = normal walk/fly logic; else lean bank + frame. */
export function groundStateSprite(
  s: GroundState,
): { bank: "start" | "sair"; side: "r" | "l"; frame: number } | null {
  switch (s.name) {
    case "start-r":
    case "stop-r":
      return { bank: "start", side: "r", frame: s.shp >> 1 };
    case "start-l":
    case "stop-l":
      return { bank: "start", side: "l", frame: s.shp >> 1 };
    case "sair-r":
    case "eair-r":
      return { bank: "sair", side: "r", frame: s.shp >> 1 };
    case "sair-l":
    case "eair-l":
      return { bank: "sair", side: "l", frame: s.shp >> 1 };
    default:
      return null;
  }
}
