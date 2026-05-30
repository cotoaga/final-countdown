# Final Countdown — web reincarnation (drop-in)

Recovered source + assets for the 1990 Demonware Amiga game *Final Countdown* (Kurt co-designed it),
prepared for the web reincarnation. Drop the contents of this bundle into your `FC/` repo root.

## Start here
1. Read **`CLAUDE.md`** — standing house conventions (Surgical Discipline, via negativa, pipeline shape).
2. Read **`reference/move-physics.md`** — the jetpack/fuel physics, extracted from the original source.
3. Work the directive: **Linear COT-55** (CotoagaDotNet) — playable Deck-1 floor.

## Contents
```
CLAUDE.md                     standing tactical calibration (read every session)
reference/
  src/                        original C + 68000 source (move.c = physics truth, foes.c = AI)
  move-physics.md             extracted physics constants
assets/raw/                   SOURCE ART — IFF ILBM, inputs to the asset pipeline (never hand-edit)
  girl/                       Laira: walk a/b, air e/f, death d.*, etc.
  foes-deck1/                 Deck-1 robots, 8 enemies × 8 frames
  pics/                       Titelscreen, Computer, ComputerTercom, Asteroid, Background, demonlogo
  obj/                        energy cell, mines (active/wait), disk, font, digits
  deck1/                      Deck-1 tiles (base1) + elements (elem1)
  blueprints/                 riss/ vessel + deck schematics (lore/UI later)
spike/
  final_countdown_slice.html  disposable feel-reference (validated). Do NOT port verbatim.
```

## The one rule that matters
The spike proved the *feel* ("feels like Laira Tyrik"). Your job is to rebuild it clean per COT-55,
not to copy the spike. Physics from `move.c`. Assets from a deterministic ILBM pipeline. Box defined
in the ticket; fill the box.
```
Decode proven with:  ilbmtoppm -adjustcolors <file> | pnmtopng   (netpbm)
```
