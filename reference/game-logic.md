# Game Logic Archaeology — The Final Countdown (Demonware 1989/90)

Reconstructed from `reference/src/{cli.c,comp.c,main.c,devices.c,start.c}`. Every claim traces to a call site.

## The Situation

Laira Tyrik is aboard the **VARGHUR**, a hostile ship run by the master computer **HYGOR**, on course for the "target planet" (10,000 BIL KM out, `POSITION` command). The ship carries the **NASKUR-DETONATOR** — a nova bomb. Mission: sabotage the ship's systems via its own computer network, abort the bomb, trigger self-destruction, and beam out alive.

## The Doom Engine: `itime`

One master counter drives everything (`no_name()` in devices.c, ticks with score):

- Once nonzero, `itime` increments every tick. **At 10000 → `novbom=1`** (nova bomb ignition, red alert, flash) → 30 ticks later → `apokalypse()`.
- Every sabotage virus **raises the itime floor** — each successful strike accelerates the countdown. Sabotage has a price: 2000 → 8000 → 9500 → 9800.
- `steexc==200` → apokalypse (runaway engine burn). `seldes==200` → apokalypse (you triggered self-destruct and didn't leave).
- Intruder alert (`intal`): wrong password → `intal=1`; at 10 ticks unresolved → `itime=9998`, `fatal=5`. The `DERUR` program (Disk 4) cancels an alert if run before escalation.
- `fatal != 0` = HYGOR override: all external computer access dies ship-wide (`check_user()`), STATUS shows SYSTEM FAILURE. Game unwinnable, walking dead.

## The Computer Network

- Every deck has terminals (`comp[]` per level). Access = `use_dev()` standing at the terminal.
- **Two password layers**: protected files ask per-level file codes (`passfile`: DOPHAZ, ITLARS, DESKIDE, F4–F7, CORMUR); protected devices ask per-level device codes (`passcomp`: NASKUR, WPERYOD, AXYRN, SARDANYX, CUTHUR, NASKUR, ENOBYNLEM, KALATORYE). Answers live in the TXT files — read the ship's own documentation to hack it.
- **8 disks** hidden as pickups on the decks (`MDISK`, `disk |= 1<<level`). Each disk = one directory (`CD DISK n`) holding one weapon-program + docs. The `.PRG` files only *execute* on one specific terminal: `(level == N && no == M)`.

## The Sabotage Chain (per-program, from cli.c)

| Program | Disk | Runs ONLY at | Effect | itime floor |
|---|---|---|---|---|
| ORDER298 / ORDER096 | 7 | L2 comp 8 | `semcom=1` — message sent via SEMORCOM. **Requires HYGOR alive** (`!hygkoo`) — sequencing constraint | — |
| DERURTACHYN | 1 | L2 comp 7 | Tachyon-Desolator OFF + energy platform deck 7 disabled | 2000 |
| DERURASSIM | 2 | L6 comp 10 | Solar-Assimilator OFF, gate deck 7 opens (`dev_off(20)`), `steexc=1` — VARGHUR burns away from Sol (200-tick doom clock!) | 8000 |
| DERUREKIN | SYSTEM | L3 comp 2 | Requires `steexc`. Stops Ekin-Manipulator, `ekiman=1`, **`steexc=0`** — kills the runaway burn | 9500 |
| MOEBIUS | 4 | L6 comp 8 | Corrupts ENERK supervision → all Enerk foes disabled → `tarpen=1` overheat cascade | (chain → 9800) |
| DERURNSKR | 5 | L4 comp 5 | **Requires `hygkoo`**. Aborts Naskur-Detonator: `novbom=itime=0`, `seldes=1` — self-destruct, 200 ticks | resets to 0 |
| CORURDEIMAR | 6 | L7 comp 6 | DEIMAR transporter setup: mass, 3 destination coords, time offset → `ttime` | — |
| CLIMATE | 8 | L5 comp 4 | Atmosphere mixer per deck, 5 elements summing to 10; safe mix: `e[1]+e[2]==10, e[2]∈[2,3]` — cures green-gas decks; wrong mix poisons them | — |
| MGARRYN | SYSTEM | L7 comp 11 | Retargets the Multi-Garryn exit to any deck 1–7 | — |

## The HYGOR Kill — the "unswitchable" device

**There is no DERURHYGOR.** The master computer cannot be commanded off. MOEBIUS silences it indirectly (`no_name()` cascade in devices.c):

```
MOEBIUS → tarpen=1 → 20 ticks → TARPEN OVERHEATING, itime≥9800, light_puls, viccol=1
        → 20 ticks → VICHOROFF DESTROYED, power to RESERVE, hygkoo=1
        → HYGOR: DISABLED, lights go out ship-wide (light_off)
```

Only with HYGOR down does DERURNSKR work. Running it while HYGOR lives = `fatal=3`, intruder alert class II, escape passages sealed (`dev_on(14..17)`).

## The Bait Viruses

The disks contain programs named for systems — some are **traps even on their matching terminal**:

- **DERURVIHF** (Disk 3, L1 comp 10) → `fatal=2`. The Vichoroff dies via the MOEBIUS cascade, never via its own virus.
- **DERURTRN** (Disk 3, L7 comp 4) → `fatal=1`. Pure bait.
- **MOEBIUS at L7 comp 9** → `fatal=4`. Right program, wrong terminal, dead.

Design signature: the ship's filesystem lies to you. Reading the docs is survival; running programs on faith is death.

## Victory Condition (comp.c:488)

```c
if (seldes && semcom && trans == 2 && mass == 76 && dka == 3263 && dkb == 3608 && dkc == 4433)
```

1. Self-destruct running (`seldes`) — DERURNSKR after the HYGOR kill
2. Message sent (`semcom`) — ORDER298 while HYGOR still lived
3. Beamed out (`trans==2`): at `score == ttime` → `trans=1`; Laira must stand on **level 1 (deck 2), x∈(50,70)** → transport fires, `trans=2`
4. DEIMAR configured **exactly**: mass **76** (Laira's mass!), destination **3263/3608/4433**
5. Bonus grading: `disk == 0xFF` (all 8 disks) removes a reproach line in the ending text

Get the coordinates wrong and the transporter still fires — the ending text tells you what arrived at the other end wasn't intact. Endings are graded, not binary.

## Open Threads

- `enigma1()..enigma8()` calls are commented out throughout — a removed layer (copy-protection retaliation? Easter eggs?). Cross-ref with COT-81 findings.
- German check questions (`chklin1/chklin2`: "WIEVIELE BEFEHLE SIND IN DER...") — manual-lookup copy protection inside the CLI.
- `infected[level] |= 1<<no` (moebius) — tracked per-computer infection state, display use TBD.
- TXT content files (Msg0..A, Txt*, Sys*, Dsk*) hold the actual fiction — extraction pending, they're on the ADFs.

## Deck animation — copper colour cycling, not tile swapping (COT-254, 2026-11-08)

**One ambient mechanism: the palette moves, the tiles don't.** `cycle_col()` (main.c:468-499) runs once per inner-loop tick (main.c:298) and pokes colour values straight into the playfield copper list.

- **Which slots.** `colorptr = colptr = copperlist+1; colptr += 2*11` (start.c:1029-1030). Each copper MOVE is two words, so `colptr` is the value word of **COLOR11**, and `*(colptr+2k)` is COLOR(11+k).
  - COLOR11–14 ← `cyccol[level][colcounter3][0..3]` (main.c:477-480).
  - COLOR15 ← `*(colptr+8)` (main.c:489-491): a ramp. Levels 4–6 `<<4` (green), level 7 `>>1` (blue), else `(colcounter>>1)<<8` (red; deck 1 = level 0).
- **Cadence.** `if (++count > 4)` (main.c:474): the four-colour rotation advances on every **5th** call = 10 Hz at 50 Hz, phase order 0,1,2,3 (`colcounter3 &= 3`, main.c:481-482). The first write lands on call 5, so for ~0.1 s after entry COLOR11–14 hold the Struct colours. The COLOR15 ramp moves **every** call: `colcounter` bounces 0↔31 (turns when `> 2*15`, and when `< 1`, main.c:485-488), colour = `colcounter>>1`, full period 62 ticks ≈ 1.24 s.
- **Table.** `cyccol[8][4][4]`, main.c:47-79. Level 0 = `066 099 0cc 0ff` rotated one step per phase (cyan family). Quoted verbatim in `src/play/cycle.ts`; the table is `WORD` 12-bit RGB, no nibble tricks (`0x0cc` = R0 G12 B12).
- **Who rides those slots.** Elements0 (deck 1): 97 of 283 tiles contain pixels on indices 11–15 — the gate glow (MFLASH 21+ / MGFLASH 63+ tiles, comp.c:73-80,152-159), wall lights, panel details — plus the MTRAP/MFUEL/MDISK1 item bobs. All of it animates by this one rotation. Decoded 2026-11-08.
- **Gates are not frame-animated.** MFLASH/MGFLASH tile numbers appear only in comp.c (initial placement) and devices.c:305-328 (`remake_ground` on dev_on/dev_off). That is an **on/off device state swap** (M2 sim), not an animation loop. The shimmer on a live gate is the colour cycle.
- **Mines.** `copy_obj` (trans.asm:41-92) blits a 16×16 masked bob per item field: `(cell&$3800)>>9 − 4` → `objmem[]`: MMINE 0, MTRAP 1, MFUEL 2, MDISK1 3 (objects 194 B each after the 720 B sintable in gfx/Stuff, start.c:462-477). MDISK2–4 would index past the 4-entry array; never drawn. Pixel-verified: **object 0 = `obj/mine.wait`** (MMINE: a laid mine counting down, `countdown = 300`, move.c:396-399; no cycled colours), **object 1 = `obj/mine.active`** (MTRAP: the armed trap `devices.c:212` converts it to after the countdown; its core is colours 11–15). There is no swap between them per frame: the "pulse" of an armed mine is the colour cycle. `wait → active` is the one-way MMINE→MTRAP conversion (M2).
- **HUD, not ported.** `*(colptr2-2)` (main.c:493-498) pulses status-bar colour 30 between `statr/statg/statb`-masked ramps (colcounter2 5..15); the status bar has its own copper palette (start.c:1009-1010). Our HUD is drawn in canvas chrome, so this is not implemented.
- **Lifts** move as gameplay (move.c tthrust/LIFTLIFE) — M2, not here.
