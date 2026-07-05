// Jetpack/fuel physics — verbatim port of grav_sim() / add_x() / add_check_y()
// in reference/src/move.c (summarized in reference/move-physics.md). Units:
// xp 1/32 px, yp 1/64 px, velx/vely same sub-pixel units. This module is the
// one place canon constants live — the tuning panel mutates a runtime copy,
// never CANON_CONSTANTS.
export interface FlightConstants {
  thrust: number; // vely -= thrust per thrust-frame (fuel > 0)
  gravity: number; // vely += gravity otherwise, up to velyClamp
  friction: number; // velx decay/turnaround step per frame
  velyClamp: number;
  velxClamp: number;
}

// move.c: --vely / vely+=1 / velx+=-1|+1 / clamp ±64 / clamp ±32.
export const CANON_CONSTANTS: FlightConstants = {
  thrust: 1,
  gravity: 1,
  friction: 1,
  velyClamp: 64,
  velxClamp: 32,
};

export interface FlightBody {
  xp: number;
  yp: number;
  velx: number;
  vely: number;
  fuel: number;
}

export interface FlightInput {
  joyx: -1 | 0 | 1;
  thrust: boolean;
}

function clamp(v: number, lo: number, hi: number): number {
  return v < lo ? lo : v > hi ? hi : v;
}

/** One 50 Hz logic tick. Pure — no DOM, no globals, no mutation of inputs. */
export function stepFlight(b: FlightBody, input: FlightInput, c: FlightConstants): FlightBody {
  let { velx, vely, xp, yp, fuel } = b;

  // grav_sim(): thrust accel == gravity accel (1:1) — the signature floatiness.
  if (input.thrust && fuel > 0) {
    vely -= c.thrust;
    fuel -= 1;
  } else if (vely < c.velyClamp) {
    vely += c.gravity;
  }
  vely = clamp(vely, -c.velyClamp, c.velyClamp);

  // add_x(): accel toward joyx, decay to 0 when released, snappy turnaround.
  velx += input.joyx;
  if (input.joyx === 0) {
    if (velx > 0) velx -= c.friction;
    else if (velx < 0) velx += c.friction;
  } else if (velx > 0 && input.joyx < 0) velx -= c.friction;
  else if (velx < 0 && input.joyx > 0) velx += c.friction;
  velx = clamp(velx, -c.velxClamp, c.velxClamp);

  xp += velx;
  yp += vely;
  if (yp < 0) {
    yp = 0;
    vely = 0; // add_check_y(): ceiling
  }

  return { xp, yp, velx, vely, fuel };
}
