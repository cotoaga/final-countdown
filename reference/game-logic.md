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
