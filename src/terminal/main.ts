// A9000 terminal slice — renders the original computer chassis with a live CLI
// inside the CRT. The deck dropdown boots that deck's protected computer
// (pcomno), so the access codes, deck/section reports and gated programs all
// change per deck. Command behaviour: src/terminal/commands.ts.
//
// Output reveals line-by-line (the original spaced its boot/contact lines with
// delays) so the screen visibly works rather than dumping instantly.
import { asset, loadImage } from "../shared/assets";
import { BitmapFont } from "../shared/font";
import { startLoop } from "../shared/loop";
import { guard } from "../shared/stage";
import { boot, handle, newState, type TerminalState } from "./commands";

const CHASSIS = "computer";
const PHOSPHOR = "#cfe0ff";
const PAD = 6;
const LINE_H = 10;
const REVEAL_EVERY = 5; // ticks per revealed line (~100ms)

const canvas = document.getElementById("screen") as HTMLCanvasElement;
const ctx = canvas.getContext("2d")!;
ctx.imageSmoothingEnabled = false;
const deckSel = document.getElementById("deck") as HTMLSelectElement;

let state: TerminalState;
let log: string[] = []; // revealed lines on screen
let feed: string[] = []; // lines waiting to be revealed (typewriter cadence)
let input = "";
let blink = 0;
let revealCd = 0;
let terminated = false;

const screen = asset(CHASSIS).screen!;
const cols = Math.floor((screen.w - PAD * 2) / 8);
const rows = Math.floor((screen.h - PAD * 2) / LINE_H);

const booting = () => feed.length > 0;

function queue(lines: string[]) {
  for (const l of lines) {
    if (l === "\f") { feed.push("\f"); continue; } // CLS marker
    if (l.length <= cols) feed.push(l);
    else for (let i = 0; i < l.length; i += cols) feed.push(l.slice(i, i + cols));
  }
}

function bootDeck(level: number) {
  state = newState(level);
  log = [];
  feed = [];
  input = "";
  terminated = false;
  revealCd = 0;
  queue(boot(state));
}

function submit() {
  if (booting()) return; // ignore input mid-reveal
  if (terminated) { bootDeck(state.level); return; }
  queue([input]); // echo the typed line
  const r = handle(state, input);
  input = "";
  queue(r.lines);
  if (r.end) { queue(["ACCESS TERMINATED"]); terminated = true; }
}

window.addEventListener("keydown", (e) => {
  if (e.key === "Enter") { e.preventDefault(); submit(); return; }
  if (booting()) return;
  if (e.key === "Backspace") { e.preventDefault(); input = input.slice(0, -1); return; }
  if (e.key.length === 1 && !e.ctrlKey && !e.metaKey && !e.altKey) {
    const ch = e.key.toUpperCase();
    if (/[A-Z0-9 ./:%!?=-]/.test(ch) && input.length < cols - 1) input += ch;
  }
});

deckSel.addEventListener("change", () => bootDeck(parseInt(deckSel.value, 10)));

guard(canvas, async () => {
  const chassis = await loadImage(CHASSIS);
  const font = await BitmapFont.load();
  bootDeck(0);

  const tick = () => {
    blink = (blink + 1) % 50;
    if (feed.length && --revealCd <= 0) {
      revealCd = REVEAL_EVERY;
      const next = feed.shift()!;
      if (next === "\f") log = [];
      else log.push(next);
    }
  };

  const render = () => {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.drawImage(chassis, 0, 0);

    const x0 = screen.x + PAD;
    const y0 = screen.y + PAD;
    const visibleLog = log.slice(-(rows - 1)); // keep last row for input
    let y = y0;
    for (const l of visibleLog) {
      font.draw(ctx, l, x0, y, PHOSPHOR);
      y += LINE_H;
    }
    const showCursor = blink < 25;
    if (booting()) {
      if (showCursor) { ctx.fillStyle = PHOSPHOR; ctx.fillRect(x0, y, 7, 8); }
    } else if (!terminated) {
      font.draw(ctx, input, x0, y, PHOSPHOR);
      if (showCursor) { ctx.fillStyle = PHOSPHOR; ctx.fillRect(x0 + input.length * 8, y, 7, 8); }
    } else if (showCursor) {
      font.draw(ctx, "PRESS ENTER TO REBOOT", x0, y, "#ff8a00");
    }
  };

  startLoop(tick, render);
});
