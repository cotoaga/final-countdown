# Decoded Audio — The Final Countdown (Demonware 1989/90)

Extracted from `assets/raw/audio/{Sound,SSound,title}` and the 1990 Aztec C source.

## Formats
- **Sound** (96,550 B) = 21 concatenated raw signed 8-bit PCM samples. Cut points
  come from the `sound[]` table in `reference/src/start.c` — byte-exact match.
- **SSound** (33,124 B) = 4 samples, same scheme, `syssound[]` table. Byte-exact.
- **title** (88,490 B) = NoiseTracker/ProTracker M.K. module, 12 patterns,
  27 positions, 6 instruments. "countdown title" by Torsten Gellrich, credits
  embedded in sample names. Replayed in-game by the Mahoney & Kaktus
  NoiseTracker V1.0 routine (`reference/src/noisereplay.asm`).
- Playback rate: PAL Paula, rate = 3546895 / period. Each WAV is exported at
  its native rate (2072–8287 Hz).

## Engine (do_sound, externals.c)
- 4 Paula channels. **Channel 0 is reserved**: only sound 4 (background
  ambience) plays there, forced at level start.
- Channels 1–3: `do_sound(nr)` picks the busy channel with the LOWEST current
  priority and steals it only if `sound[nr].pri >=` that channel's priority.
- Priority 3 sounds refuse to double up (one instance max).
- `do_sound(-n)` plays `syssound[n-1]` (system computer sounds).
- `kill_chan_with_pri(p)` silences channels holding priority p — used to cut
  the text-scroll drone (sfx16, pri 13) when a page finishes.
- `apokalypse()` seizes all 4 channels with a silent buffer — total blackout.

## Event map (from all C call sites)
See manifest.json `usage` fields. Highlights:
- sfx00 pickup/collect · sfx01 death/explosion · sfx03 thrust ignition
- sfx04 background ambience (ch0) · sfx05 lift · sfx06 message arrival
- sfx11 foe kill · sfx13/14 device on/off · sfx16 text scroll
- sfx18 key click + radiation tick · sfx20 Laira's footsteps (every 32nd walk frame)
- sfx09/12/15/17/19 have NO C call sites — candidates: asm-triggered or cut content.

## Files
- `sfx/sfx00..20.wav`, `syssfx/sys00..03.wav` — decoded, native rates
- `title.mod` — original module, `title.mp3` — ffmpeg/libopenmpt render (3:27)
- `manifest.json` — table: bytes, period, rate, volume, priority, usage
