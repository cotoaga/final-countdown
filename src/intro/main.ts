// Intro sequence — the original opening, rebuilt on canvas:
//   demon logo  →  title screen  →  starfield credits (asteroid flyby +
//   colour-cycled title)  →  story crawl  →  standby.
// It AUTOPLAYS on timers (so it plays untouched); fire (space/click) skips to
// the next phase. Flow follows title_pic() + title() in reference/src.
import { asset, loadImage } from "../shared/assets";
import { BitmapFont } from "../shared/font";
import { startLoop, TICK_HZ } from "../shared/loop";
import { guard } from "../shared/stage";
import { STORY } from "./story";

// cyccol[0] VERBATIM from reference/src/main.c:47 — Amiga 12-bit, 4 rotation
// rows. (The previous four hex strings here were an invented approximation
// and a nibble-mistranslation: 0x066 is teal #006666, not blue #0066ff.)
const CYCCOL0 = [
  [0x066, 0x099, 0x0cc, 0x0ff],
  [0x099, 0x0cc, 0x0ff, 0x066],
  [0x0cc, 0x0ff, 0x066, 0x099],
  [0x0ff, 0x066, 0x099, 0x0cc],
];
const amigaRgb = (c: number): [number, number, number] => [
  ((c >> 8) & 15) * 17,
  ((c >> 4) & 15) * 17,
  (c & 15) * 17,
];
const amigaCss = (c: number): string => {
  const [r, g, b] = amigaRgb(c);
  return `rgb(${r},${g},${b})`;
};

// Title-screen asteroid effect, start.c:1250-1273 — asteroid blits into the
// window l ∈ (OFFSET, OFFSET+896] at x = ticks/2, y = 80, sweeping in DARK;
// then three register groups ignite in waves at three tempos. l is a 50 Hz
// VBL counter and our loop is 50 Hz fixed-step, so timings port 1:1.
const OFFSET = 1200;
const SWEEP_TICKS = 896;
const ASTEROID_Y = 80;
const IGNITION = [
  { at: 400, regs: [5, 6, 7, 17], shift: 3 },
  { at: 450, regs: [1, 2, 3, 4], shift: 4 },
  { at: 500, regs: [18, 29, 30, 31], shift: 2 },
];
const PHOSPHOR = "#cfe0ff";

type Phase = "logo" | "title" | "credits" | "story" | "standby";

// auto-advance after this many ticks if the player does nothing
const AUTO: Record<Phase, number> = {
  logo: Math.round(3.5 * TICK_HZ),
  title: Math.round(3.5 * TICK_HZ),
  credits: 24 * TICK_HZ,
  story: 0, // handled per-line below
  standby: 0,
};
const MIN_SKIP = 6; // ticks before fire is allowed (debounce)

const canvas = document.getElementById("screen") as HTMLCanvasElement;
const ctx = canvas.getContext("2d")!;
ctx.imageSmoothingEnabled = false;
const W = canvas.width;
const H = canvas.height;
const hint = document.getElementById("hint")!;

let fired = false;
const fire = () => (fired = true);
window.addEventListener("keydown", (e) => {
  if (e.key === " " || e.key === "Enter") { e.preventDefault(); fire(); }
});
canvas.addEventListener("pointerdown", fire);
canvas.tabIndex = 0;

// Palette-indexed sprite: pixels mapped to CMAP indices once at load, so we
// can do the two things baked-RGB PNGs cannot — Amiga register cycling and
// index-0 transparency (index 0 is transparent; "black" is irrelevant).
interface IndexedSprite {
  w: number;
  h: number;
  idx: Uint8Array;
  base: number[][];
  canvas: HTMLCanvasElement;
  image: ImageData;
}

function indexSprite(img: HTMLImageElement, palette: number[][]): IndexedSprite {
  const w = img.naturalWidth;
  const h = img.naturalHeight;
  const cv = document.createElement("canvas");
  cv.width = w;
  cv.height = h;
  const c2 = cv.getContext("2d")!;
  c2.drawImage(img, 0, 0);
  const src = c2.getImageData(0, 0, w, h);
  const idx = new Uint8Array(w * h);
  const memo = new Map<number, number>();
  for (let p = 0; p < w * h; p++) {
    const r = src.data[p * 4];
    const g = src.data[p * 4 + 1];
    const b = src.data[p * 4 + 2];
    const key = (r << 16) | (g << 8) | b;
    let best = memo.get(key);
    if (best === undefined) {
      // Nearest match absorbs ilbmtoppm's -adjustcolors rescaling.
      let bd = Infinity;
      best = 0;
      for (let i = 0; i < palette.length; i++) {
        const dr = r - palette[i][0];
        const dg = g - palette[i][1];
        const db = b - palette[i][2];
        const d = dr * dr + dg * dg + db * db;
        if (d < bd) {
          bd = d;
          best = i;
        }
      }
      memo.set(key, best);
    }
    idx[p] = best;
  }
  return { w, h, idx, base: palette, canvas: cv, image: c2.createImageData(w, h) };
}

function paintSprite(s: IndexedSprite, overrides: ([number, number, number] | null)[]): void {
  const d = s.image.data;
  for (let p = 0; p < s.idx.length; p++) {
    const i = s.idx[p];
    if (i === 0) {
      d[p * 4 + 3] = 0; // Amiga transparency: palette index 0
      continue;
    }
    const rgb = overrides[i] ?? s.base[i];
    d[p * 4] = rgb[0];
    d[p * 4 + 1] = rgb[1];
    d[p * 4 + 2] = rgb[2];
    d[p * 4 + 3] = 255;
  }
  s.canvas.getContext("2d")!.putImageData(s.image, 0, 0);
}

const HINTS: Record<Phase, string> = {
  logo: "PRESS FIRE TO SKIP",
  title: "PRESS FIRE TO SKIP",
  credits: "PRESS FIRE TO SKIP",
  story: "PRESS FIRE TO SKIP",
  standby: "PRESS FIRE — DECK 1 NOT YET BUILT",
};

guard(canvas, async () => {
  const [demonlogo, titlescreen, background, asteroid] = await Promise.all([
    loadImage("demonlogo"),
    loadImage("titlescreen"),
    loadImage("background"),
    loadImage("asteroid"),
  ]);
  const ast = asset("asteroid");
  if (!ast.palette) throw new Error("asteroid asset has no palette — rerun npm run assets");
  const sprite = indexSprite(asteroid, ast.palette);
  let cycleSig = "";
  paintSprite(sprite, new Array(ast.palette.length).fill(null)); // dark sweep state
  const font = await BitmapFont.load();

  let phase: Phase = "logo";
  let pt = 0; // ticks since this phase began
  let storyShown = 0; // lines revealed in the crawl
  let fullAt = -1; // tick when the crawl finished revealing

  const enter = (p: Phase) => { phase = p; pt = 0; fullAt = -1; hint.textContent = HINTS[p]; };
  const want = new URLSearchParams(location.search).get("phase") as Phase | null;
  const start: Phase = want && want in HINTS ? want : "logo";
  if (start === "story") storyShown = STORY.length;
  enter(start);

  const tick = () => {
    pt++;
    const skip = fired && pt > MIN_SKIP;
    fired = false;

    switch (phase) {
      case "logo":
        if (skip || pt > AUTO.logo) enter("title");
        break;
      case "title":
        if (skip || pt > AUTO.title) enter("credits");
        break;
      case "credits":
        if (skip || pt > AUTO.credits) enter("story");
        break;
      case "story":
        if (storyShown < STORY.length) {
          if (skip) storyShown = STORY.length;
          else if (pt % 9 === 0) storyShown++;
          if (storyShown >= STORY.length) fullAt = pt;
        } else {
          if (fullAt < 0) fullAt = pt;
          if (skip || pt - fullAt > 5 * TICK_HZ) enter("standby");
        }
        break;
      case "standby":
        if (skip) window.location.href = "./index.html";
        break;
    }
  };

  // fade helper: black curtain that lifts over `dur` ticks
  const fadeIn = (dur: number) => {
    const a = Math.max(0, 1 - pt / dur);
    if (a > 0) { ctx.fillStyle = `rgba(0,0,0,${a})`; ctx.fillRect(0, 0, W, H); }
  };

  const render = () => {
    ctx.fillStyle = "#000";
    ctx.fillRect(0, 0, W, H);

    switch (phase) {
      case "logo":
        ctx.drawImage(demonlogo, 0, 0, W, H);
        fadeIn(0.6 * TICK_HZ);
        break;

      case "title":
        ctx.drawImage(titlescreen, 0, 0, W, H);
        fadeIn(0.6 * TICK_HZ);
        break;

      case "credits": {
        ctx.drawImage(background, 0, 0, W, H);
        // start.c: l = OFFSET + ticks-in-window; x advances 1px per 2 ticks.
        const l = OFFSET + pt;
        if (pt > 0 && pt <= SWEEP_TICKS) {
          const overrides: ([number, number, number] | null)[] = new Array(
            sprite.base.length,
          ).fill(null);
          for (const g of IGNITION) {
            if (pt >= g.at) {
              const row = CYCCOL0[(l >> g.shift) & 3];
              g.regs.forEach((reg, k) => {
                if (reg < overrides.length) overrides[reg] = amigaRgb(row[k]);
              });
            }
          }
          const sig = overrides.map((o) => (o ? o.join() : "-")).join("|");
          if (sig !== cycleSig) {
            paintSprite(sprite, overrides);
            cycleSig = sig;
          }
          ctx.drawImage(sprite.canvas, Math.floor(pt / 2), ASTEROID_Y);
        }
        const color = amigaCss(CYCCOL0[(l >> 3) & 3][0]);
        const t1 = "FINAL COUNTDOWN";
        font.draw(ctx, t1, Math.round((W - font.measure(t1)) / 2), 28, color);
        const t2 = "DEMONWARE  1990";
        font.draw(ctx, t2, Math.round((W - font.measure(t2)) / 2), 44, PHOSPHOR);
        fadeIn(0.5 * TICK_HZ);
        break;
      }

      case "story": {
        // Crawl, not stack: once the text would run past the bottom, older
        // lines scroll off the top. (The original presented this as a crawl;
        // stacking clipped the tail of the MSGA text below the canvas.)
        const x = 20;
        const lineH = 10;
        const top = 24;
        const bottom = H - 30;
        const scroll = Math.max(0, top + storyShown * lineH - bottom);
        let y = top - scroll;
        for (let i = 0; i < storyShown; i++) {
          if (y > -lineH && y < H) font.draw(ctx, STORY[i], x, y, PHOSPHOR);
          y += lineH;
        }
        if (storyShown >= STORY.length && Math.floor(pt / 25) % 2 === 0) {
          font.draw(ctx, "PRESS FIRE", x, Math.min(y + 6, H - 14), amigaCss(0x0cc));
        }
        break;
      }

      case "standby": {
        const t = "DECK 1 - STANDBY";
        font.draw(ctx, t, Math.round((W - font.measure(t)) / 2), H / 2 - 4, PHOSPHOR);
        break;
      }
    }
  };

  startLoop(tick, render);
});
