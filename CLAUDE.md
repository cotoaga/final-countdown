# CLAUDE.md — Final Countdown (web reincarnation)

Standing tactical calibration. Read every session. This carries House Cotoaga conventions so they
do **not** drift to model defaults. Per-task directive lives in the Linear ticket (COT-55).

## What this project is
A 2026 web reincarnation of *Final Countdown* (Demonware, Amiga, 1990 — Kurt co-designed it).
Original source + assets are recovered and live in this repo. Current target: **COT-55** —
a playable Deck-1 floor with the original jetpack/fuel physics.

## Where things are
- `reference/src/` — the **original C + 68000 source**. `move.c` is the physics ground truth.
  Read it; do not re-invent behaviour you can read here. `foes.c` = robot AI for later decks.
- `reference/move-physics.md` — extracted physics constants (the short version of move.c).
- `assets/raw/` — **source art, IFF ILBM format**. `girl/`, `foes-deck1/`, `pics/`, `obj/`,
  `deck1/`, `blueprints/`. These are the inputs to the asset pipeline. NEVER hand-edit them.
- `spike/final_countdown_slice.html` — a disposable feel-reference spike (KHAOS, validated by
  Kurt as "feels like Laira Tyrik"). **Do not port verbatim.** It proves the feel; rebuild clean.

## Asset pipeline (non-negotiable shape)
ILBM → trimmed/padded PNG sheets + JSON manifest, as a deterministic `npm run assets` step.
`ilbmtoppm -adjustcolors <file> | pnmtopng` is the proven decode (netpbm). Transparency: key the
corner background colour (sprites use a transparent index). Sheets are **generated artifacts** —
committed if convenient, but always reproducible from `assets/raw/`. Re-running must be diff-clean.

## House discipline (Surgical Discipline — the quality gate)
- Every changed line traces to the directive. No drive-by refactors. No "while I'm here" cleanup.
- Match existing style even if you'd write it differently.
- Orphans you create are yours to remove; pre-existing dead code is **flagged, not deleted**.
- Unrelated issues → Linear comment, not this PR.
- Line budgets in the directive are real. Exceed = justify in the PR.
- Physics constants traceable to `move.c`. No magic numbers scattered in the loop — one named module.

## Via negativa (do NOT)
- Do NOT reimplement the 68k decompressor. Assets are plain ILBM; there is nothing to decompress.
- Do NOT add a game engine or framework. Vanilla TS + canvas, fixed-timestep loop.
- Do NOT abstract for "all 8 decks" before deck 2 exists.
- Do NOT add sound/music in the COT-55 slice.
- Do NOT build a level editor (the original's editor source is in reference/ for study only).

## Verification (both layers)
1. Self: lint → typecheck → test → green → commit. No suite yet? First commit adds meaningful
   physics tests (velx/vely clamps, friction decay, fuel burn, ground/gap collision). Zero-test
   green is theatre.
2. Post-deploy (KHAOS): browser screenshot mid-flight over the gap, JS-probe constants vs move.c.

## Git
Atomic commits: `feat|fix|refactor|docs|chore(scope): desc`. Pre-push summary as a Linear comment
on COT-55. CHANGELOG updated for user-facing changes.
