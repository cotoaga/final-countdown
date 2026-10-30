# Roadmap — from museum to game (2026-10-30)

State at writing: **M0.** Six working exhibits (intro, A9000 terminal, flight sandbox, deck viewer,
level decoder, SCHIFFSARCHIV lore, sound bank) with no wires between them. Archaeology is done
enough. The next phase is integration. Each milestone ends with a **playtest gate on prod** — Kurt
plays `final-countdown-chi.vercel.app` and says "Acceptable" or doesn't. Local green is not done.

## Via negativa (standing until M4)
- No more archaeology as primary work. DE/EN toggle, German Disk 2 hunt, Elements-bank hypothesis:
  parked. 141 English-only texts block nothing. The bank hypothesis matters when deck 2 exists.
- Do not start with foes. Sexiest module, least load-bearing.
- Do not build the 8-deck pipeline first. One deck end-to-end beats eight half-wired.
- Do not port `spike/`.
- Do not reinvent game logic that can be ported. `devices.c`/`comp.c`/`cli.c` is a state machine
  with ~30 globals. Port it; do not redesign it.

## Milestones

| M | Name | Gate (what Kurt can do on prod) | Linear |
|---|---|---|---|
| **M1** | Laira walks on real metal | Traverse Deck 1 end to end with jetpack; it feels like 1990 | COT-252 (a) · COT-253 (b) · COT-254 (c) |
| **M2** | Ship simulation, headless | Test suite plays the golden path to the victory condition with no graphics | — |
| **M3** | Deck 1 becomes a game | Pick up a disk, use a terminal, sim state changes, feel a trap | — |
| **M4** | Multi-deck traversal | Full ship walkable; all 8 disks collectible | — |
| **M5** | Foes + visible doom clock | Die every way the original kills you; sfx at their `do_sound()` sites | — |
| **M6** | Endings | Win. Win badly (wrong coords). Win with all 8 disks. Title music | — |

### M1 — Laira walks on real metal
- **M1a** glue: `designer/flight/physics.ts` off the flat floor onto decoded `Matrix0`/`Struct0`,
  deck-viewer tile renderer, collision vs `MGROUND`, kills vs `MENERGY`/`MMINE`/`MTRAP`/fall,
  respawn, fuel HUD. Hardest precedent in the project. Dev's keyboard.
- **M1b** support wheels: in-page trainer panel. Separate flag per kill cause (GOD = all), noclip,
  gravity, fuel burn, lift timers, device slots 0–31. Probes: raw-unit readout (xp/yp/velx/vely/
  fuel/SM* state/tick), tile-bit overlay (incl. unaccounted 0x0600 bits), collision box, tick
  control (pause/step/10 Hz), click-to-teleport, input recorder → replay → test fixture. State in
  URL so a prod link is a repro. The original shipped a trainer ADF; this is ours.
- **M1c** deck animation: one archaeology hour to settle tile-swap vs copper colour-cycling for
  `MFLASH`/`MGFLASH` gates, then implement exactly that. Mines pulse (obj/ active/wait).

### M2 — Ship simulation as a headless module
Port `devices.c` + `itime` doom engine + sabotage table (`game-logic.md`) into pure TS `sim/` with
zero canvas dependency. Every row of the sabotage table is a unit test ("DERURASSIM at L6 comp 10
→ `steexc=1`, floor 8000, gate 20 opens"). Archaeology becomes executable spec. Dev sets data
shapes + tick contract; porting rows is Code-delegable inside that fence.
**Runs concurrent with M1.** Converge at M3.

### M3 — Deck 1 becomes a game
Entities from `Elements`: doors, energy platforms, pickups (disk, energy cell), terminals as
`use_dev()` hotspots (stick UP at MCOMPUTER/MSWITCH, move.c:361). Wire the existing terminal
emulator to the M2 sim instead of its own state.

### M4 — Multi-deck traversal
Lifts, Multi-Garryn exit, deck transitions, sim state persists across decks. Now the 8-deck loader
earns its existence; now resolve the Elements/Foes bank hypothesis because now it blocks something.
Decks 2–8 come online as content, not new code.

### M5 — Foes + the doom clock made visible
`foes.c` robot AI, Enerk foes tied to MOEBIUS, red alert, light-off cascade, intruder alert
escalation. Sound effects wired via `public/assets/audio/manifest.json`.

### M6 — Endings
DEIMAR transport, `trans==2` zone (level 1, x∈(50,70)), graded ending texts, mass 76, all-disks
bonus line. Title MOD.

## Doctrine reminders
- Trainer (M1b) early: iteration cost kills projects, not difficulty.
- Every 1990 constant cites `file:line`, value verbatim (CLAUDE.md).
- Designer eyes are a first-class verifier; inference is not proof.
