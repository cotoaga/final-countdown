// Intro sequence — the original opening, rebuilt on canvas:
//   demon logo  →  title screen  →  starfield credits (asteroid flyby +
//   colour-cycled title)  →  story crawl  →  standby.
// Flow follows title_pic() + title() in reference/src/{start,comp}.c.
import { asset, loadImage } from "../shared/assets";
import { BitmapFont } from "../shared/font";
import { startLoop, TICK_HZ } from "../shared/loop";
import { STORY } from "./story";

// cyccol[0] from reference/src/main.c — the title colour-cycle set.
const CYCLE = ["#0066ff", "#0099ff", "#00ccff", "#00ffff"];
const PHOSPHOR = "#cfe0ff";

type Phase = "logo" | "title" | "credits" | "story" | "standby";

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

const HINTS: Record<Phase, string> = {
  logo: "CLICK / PRESS FIRE (SPACE)",
  title: "PRESS FIRE",
  credits: "PRESS FIRE TO CONTINUE",
  story: "PRESS FIRE",
  standby: "PRESS FIRE — DECK 1 NOT YET BUILT",
};

async function main() {
  const [demonlogo, titlescreen, background, asteroid] = await Promise.all([
    loadImage("demonlogo"),
    loadImage("titlescreen"),
    loadImage("background"),
    loadImage("asteroid"),
  ]);
  const ast = asset("asteroid");
  const font = await BitmapFont.load();

  let phase: Phase = "logo";
  let pt = 0; // ticks since this phase began
  let storyShown = 0; // lines revealed in the crawl

  const enter = (p: Phase) => { phase = p; pt = 0; hint.textContent = HINTS[p]; };
  // dev affordance: ?phase=credits|story|... jumps straight to a phase
  const want = new URLSearchParams(location.search).get("phase") as Phase | null;
  const start: Phase = want && want in HINTS ? want : "logo";
  if (start === "story") storyShown = STORY.length;
  enter(start);

  const tick = () => {
    pt++;
    const consumed = fired;
    fired = false;

    switch (phase) {
      case "logo":
        if (consumed && pt > 8) enter("title");
        break;
      case "title":
        if (consumed && pt > 8) enter("credits");
        break;
      case "credits":
        // advance on fire, or auto-advance after ~24s
        if ((consumed && pt > 8) || pt > 24 * TICK_HZ) enter("story");
        break;
      case "story":
        if (consumed) {
          if (storyShown < STORY.length) storyShown = STORY.length; // reveal all
          else enter("standby");
        } else if (pt % 9 === 0 && storyShown < STORY.length) {
          storyShown++;
        }
        break;
      case "standby":
        if (consumed && pt > 8) window.location.href = "./index.html";
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
        // asteroid drifts left->right across the lower field, wrapping
        const span = W + ast.w;
        const ax = ((pt * 0.6) % span) - ast.w;
        ctx.drawImage(asteroid, Math.round(ax), 150, ast.w, ast.h);
        // colour-cycled title, centred near the top
        const color = CYCLE[Math.floor(pt / 6) % CYCLE.length];
        const t1 = "FINAL COUNTDOWN";
        font.draw(ctx, t1, Math.round((W - font.measure(t1)) / 2), 28, color);
        const t2 = "DEMONWARE  1990";
        font.draw(ctx, t2, Math.round((W - font.measure(t2)) / 2), 44, PHOSPHOR);
        fadeIn(0.5 * TICK_HZ);
        break;
      }

      case "story": {
        // A9000-style crawl: revealed lines, top-down
        const x = 20;
        let y = 24;
        for (let i = 0; i < storyShown; i++) {
          font.draw(ctx, STORY[i], x, y, PHOSPHOR);
          y += 10;
        }
        if (storyShown >= STORY.length && Math.floor(pt / 25) % 2 === 0) {
          font.draw(ctx, "PRESS FIRE", x, y + 6, CYCLE[2]);
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
}

main();
