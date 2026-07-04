# Final Countdown — Recovered Original Data (gap-closure payload)

**Recovered:** 2026-07-04, KHAOS-Dev session (Fable 5), from Kurt's original archives.
**Sources:** `AMIGA.zip` (dev LZH archives, timestamps Oct 1989–Apr 1990) + `Final_Countdown.zip` (release ADFs incl. PDX Disk 2).
**Purpose:** closes the three honest gaps flagged in PR #1's pre-push summary (COT-55).

## Gap closure map

| PR #1 flagged gap | Closed by | Location |
|---|---|---|
| Story crawl PLACEHOLDER (`DISK01:TXT/MsgA` absent) | Real MSGA recovered — full KI GENYUS log, AST2437/07-01, Laira's transmission | `txt-disk1/MSGA` |
| `Struct0N` level files absent — device layouts ILLUSTRATIVE | Complete per-deck set: `Struct0–7`, `Matrix0–7`, `Elements0–3`, `Foes0–3` | `lev/` |
| `.TXT`/`.PIC` contents out of scope | Full text trees from both disks: Txt*/Pic*/Sys*/Dsk*/MSG* | `txt-disk1/`, `txt-disk2/` |

## Contents

- `lev/` — binary level data, Disk 2 (`DISK01` volume). Format decodable via `src/cli.c`, `src/comp.c` (readers) and the FCDECK dev archives (base/elem pairs).
- `txt-disk1/`, `txt-disk2/` — in-game text + Pic files (Pic* are IFF ILBM or custom; fingerprint before assuming).
- `src/` — **complete original source.** Aztec/Manx C + 68k asm + original makefile. Link target `space`. Physics ground truth: `move.c`. CLI/terminal: `cli.c`, `comp.c`.
- `sprites-laira/` — 113 IFF ILBM frames (a/b banks, 32×42, 32-colour). Walk cycle verified rendering via `ilbmtoppm`.
- `pics-dev/` — dev-archive pics: Titelscreen, demonlogo, Computer/ComputerTercom (640px), Asteroid, Background.

## Decode notes

- All ILBM: `ilbmtoppm -adjustcolors FILE | pnmtopng` (proven pipeline, deterministic).
- `Matrix*` are uniformly 3072 bytes = likely 128×12 WORDs — cross-check against `matrix [15][128]` in `main.c`.
- Dev deck archives (FCDECK1/2/5/8) contain `baseN`/`elemN` trees — the editor-side source for the packed `Elements*` files. Editor source lives in the FCEDWRTS archive (`Space/edit/`, `Space/write/`).
- German is the game's language. Preserve verbatim; translation is a rendering concern, not a data concern.

**Provenance rule:** these are 1989–1992 originals. Never hand-edit; treat as read-only inputs to `npm run assets`-style deterministic steps.
