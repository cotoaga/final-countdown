# move-physics.md — jetpack/fuel model, extracted from original `reference/src/move.c`

The slice must feel like the 1990 original. These are the actual rules. Source: `grav_sim()`,
`add_x()`, `add_check_y()` in move.c. Tick at the original PAL rate feel (50Hz logic).

## State
- `xp` — horizontal position, **1/32 px units**
- `yp` — vertical position, **1/64 px units**
- `velx`, `vely` — velocities in the same sub-pixel units
- `fuel` — integer, drains on thrust
- `joyx` ∈ {-1,0,1} (left/right), `joyy` = -1 while thrust held

## Vertical — gravity & jetpack (`grav_sim`)
```
if (thrust held && fuel > 0) {
    vely -= 1;            // upward accel
    fuel -= 1;            // one unit per thrust-FRAME
} else {
    if (vely < 64) vely += 1;   // gravity accel
}
clamp vely to [-64, +64];
```
Key feel: **thrust accel == gravity accel (1:1)**. Hovering is an active pulse, not a hold.
This is the signature floatiness — preserve it exactly. Do not "improve" it.

## Vertical — apply (`add_check_y`)
```
yp += vely;
if (yp < 0) { yp = 0; vely = 0; }          // ceiling
// floor / fatal handled by ground + collision check
```

## Horizontal — accel, friction, clamp (`add_x`)
```
velx += joyx;
if (joyx == 0) velx -> decay toward 0 by 1 per frame;     // friction
else if reversing direction, extra ±1;                    // snappy turnaround
clamp velx to [-32, +32];
xp += velx;
```

## Screen mapping
`screen_x = xp / 32`, `screen_y = yp / 64`. Both ≈ 1 px/frame at max → symmetric on screen,
asymmetric in raw units. Play area height ≈ 180 px (`yp` max = 180*64).

## Fuel
1 unit burned per thrust-frame. Energy cells (`assets/raw/obj/energy`) refill. Tune refill amount
to taste; original gameplay rewards careful, intermittent thrust.

## Animation banks (from move.c externs + set_bob state machine, verified 2026-07-05)
- `girlgr[16]` / `girlgl[16]` — walk right/left → raw groups `b` / `a`
  (CORRECTED: `a` faces LEFT — the original a→right mapping here was an
  alphabetical guess, falsified on screen by the designer)
- `girlar[4]` / `girlal[4]` — air (jetpack) right/left → raw groups `f` / `e` (same correction)
- `girlgsr/gsl[8]` — ground START/STOP right/left (SMSTARTR/L, SMSTOPR/L — not "shoot")
- `girlasr/asl[8]` — air start/end right/left (SMSAIRR/L, SMEAIRR/L)
- `girltr[8]` — rotate-toward-camera bank, reused for THREE contexts:
  use-device rotation (SMTURNS/SMTURNE — triggered by stick UP at an
  MCOMPUTER/MSWITCH tile, `use_dev()` fires at the apex, move.c:361), hide/enter
  (SMHIDEB/E), lift ride (SMLIFTUP/DN, frame 7 held while moving). NOT a
  direction-reversal animation — an earlier note here claimed that; falsified
  against move.c:361 on 2026-07-05.
- **Direction reversal at speed = SMSTOP → SMSTART chain** (move.c:410-431):
  |velx| ≥ 15 with stick released/opposed → SMSTOP (girlgs frames backward,
  stick locked, friction brakes) → velx 0 → SMSTART opposite (frames forward,
  run forced, 16 ticks). Below the threshold: instant flip, no ceremony.
  Raw groups: `c` → gsr (RIGHT), `d` → gsl (LEFT) — designer-falsified the first
  assignment on screen, anchor-verified 2026-07-06.
- **Air reversal is the same chain in the sky** (move.c:281-299): SMFLY at
  |velx| ≥ 15, stick off/opposed → SMEAIRR/L (air brake, stick locked) →
  velx 0 → SMSAIRR/L (lean into new flight direction). One faithful
  difference: THRUST STAYS LIVE in all air states (move.c:92-121 — no joyy
  lock; the jetpack never locks, only the stick).
- Raw groups `h` → asr (RIGHT), `i` → asl (LEFT) — jetpack-flame lean poses,
  anchor-verified against a00, 2026-07-06.
- **LETTER PREFIXES CARRY NO FACING CONVENTION** (a=L b=R, c=R d=L, e=L f=R,
  h=R i=L). Any facing claim requires anchor comparison against a verified
  frame — the "alphabetical pairs" rule was falsified 2026-07-06.
- Raw group `g` (16 frames) — front-facing rotation sequence: the dev source of
  `girltr[8]` (16 drawn, 8 shipped). Unclaimed-group mystery closed 2026-07-05.
- `girld[4]` — take/up/down poses (SMTAKEU/D, SMUP, SMDOWN — not death) → raw `d.` group
- `girle[21]` — die AND appear (SMDIE, SMAPP; girlshp caps at 81 → 21 frames @ >>2) → raw `j` group
Frame dims: girl ~32×42, bottom-align on the feet baseline.
