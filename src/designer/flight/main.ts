// Flight Lab — designer sandbox for Kurt to feel Laira's jetpack/fuel physics
// against a flat floor with exactly one jetpack-mandatory gap. Canon (see
// physics.ts) is the anchor; sliders mutate a runtime copy only, and
// "1990 RESET" snaps back to it exactly.
import { BitmapFont } from "../../shared/font";
import { startLoop } from "../../shared/loop";
import { guard } from "../../shared/stage";
import { initAnim, frameIndex, stepAnim } from "./anim";
import { groundStateSprite, initGroundState, stepGroundState } from "./ground-states";
import { CANON_CONSTANTS, stepFlight, type FlightBody, type FlightConstants } from "./physics";
import { SpriteBank } from "./sprites";
import { applyWorld, isOverGap, makeWorld } from "./world";

const FUEL_MAX = 200;
const PHOSPHOR = "#cfe0ff";

const canvas = document.getElementById("screen") as HTMLCanvasElement;
const ctx = canvas.getContext("2d")!;
ctx.imageSmoothingEnabled = false;
const refuelBtn = document.getElementById("refuel") as HTMLButtonElement;
const resetBtn = document.getElementById("reset1990") as HTMLButtonElement;

const SLIDERS: Array<{ id: string; key: keyof FlightConstants }> = [
  { id: "thrust", key: "thrust" },
  { id: "gravity", key: "gravity" },
  { id: "friction", key: "friction" },
  { id: "velx-clamp", key: "velxClamp" },
  { id: "vely-clamp", key: "velyClamp" },
];

let constants: FlightConstants = { ...CANON_CONSTANTS };
const world = makeWorld();

function syncSliderUI(): void {
  for (const s of SLIDERS) {
    const input = document.getElementById(s.id) as HTMLInputElement;
    const out = document.getElementById(`${s.id}-v`) as HTMLOutputElement;
    input.value = String(constants[s.key]);
    out.textContent = String(constants[s.key]);
  }
}

for (const s of SLIDERS) {
  const input = document.getElementById(s.id) as HTMLInputElement;
  input.addEventListener("input", () => {
    constants = { ...constants, [s.key]: Number(input.value) };
    document.getElementById(`${s.id}-v`)!.textContent = input.value;
  });
}
resetBtn.addEventListener("click", () => {
  constants = { ...CANON_CONSTANTS };
  syncSliderUI();
});

let body: FlightBody = { xp: world.spawnXp, yp: world.spawnYp, velx: 0, vely: 0, fuel: FUEL_MAX };
const refuel = () => (body = { ...body, fuel: FUEL_MAX });
refuelBtn.addEventListener("click", refuel);

const keys = new Set<string>();
const keyName = (k: string) => (k === " " ? "space" : k.toLowerCase());
window.addEventListener("keydown", (e) => {
  if (e.target instanceof HTMLInputElement) return;
  const k = keyName(e.key);
  if (["arrowleft", "arrowright", "arrowup", "space", "a", "d", "r"].includes(k)) e.preventDefault();
  keys.add(k);
  if (k === "r") refuel();
});
window.addEventListener("keyup", (e) => {
  if (e.target instanceof HTMLInputElement) return;
  keys.delete(keyName(e.key));
});

let anim = initAnim();
let ground = initGroundState();

guard(canvas, async () => {
  const font = await BitmapFont.load();
  const [walkR, walkL, airR, airL, startR, startL] = await Promise.all([
    SpriteBank.load("girl-walk-r"),
    SpriteBank.load("girl-walk-l"),
    SpriteBank.load("girl-air-r"),
    SpriteBank.load("girl-air-l"),
    SpriteBank.load("girl-start-r"),
    SpriteBank.load("girl-start-l"),
  ]);

  syncSliderUI();

  const tick = () => {
    const joyx = keys.has("arrowleft") || keys.has("a") ? -1 : keys.has("arrowright") || keys.has("d") ? 1 : 0;
    const thrustHeld = keys.has("arrowup") || keys.has("space");
    // specmv ground subset runs first and may override the stick (move.c order:
    // state machine before movement — start forces the run, stop locks controls).
    const onGround = body.yp >= world.floorYp;
    const g = stepGroundState(ground, body.velx, joyx, thrustHeld, onGround);
    ground = g.next;
    body = stepFlight(body, { joyx: g.input.joyx, thrust: g.input.thrust }, constants);
    body = applyWorld(body, world);
    const flying = body.yp < world.floorYp;
    anim = stepAnim(anim, body.velx, flying);
  };

  const render = () => {
    ctx.fillStyle = "#0a0a3c";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    const floorScreenY = Math.round(world.floorYp / 64);
    const gapStart = Math.round(world.gapStartXp / 32);
    const gapEnd = Math.round(world.gapEndXp / 32);
    ctx.fillStyle = "#2b2f6e";
    ctx.fillRect(0, floorScreenY, gapStart, canvas.height - floorScreenY);
    ctx.fillRect(gapEnd, floorScreenY, canvas.width - gapEnd, canvas.height - floorScreenY);

    const flying = body.yp < world.floorYp;
    const gs = flying ? null : groundStateSprite(ground);
    const bank = gs
      ? gs.side === "r"
        ? startR
        : startL
      : flying
        ? anim.facing === 1
          ? airR
          : airL
        : anim.facing === 1
          ? walkR
          : walkL;
    const frame = gs ? gs.frame : frameIndex(anim, body.velx);
    const screenX = Math.round(body.xp / 32 - bank.frameW / 2);
    const screenY = Math.round(body.yp / 64 - bank.frameH);
    bank.draw(ctx, frame, screenX, screenY);

    font.draw(ctx, "FUEL", 6, 6, PHOSPHOR);
    const barX = 42;
    const barW = 80;
    ctx.strokeStyle = PHOSPHOR;
    ctx.strokeRect(barX, 6, barW, 8);
    ctx.fillStyle = body.fuel > 0 ? "#ff8a00" : "#602020";
    ctx.fillRect(barX + 1, 7, Math.round((barW - 2) * (body.fuel / FUEL_MAX)), 6);

    if (isOverGap(body.xp, world) && body.yp > world.floorYp) {
      font.draw(ctx, "FALLING", 6, 18, "#ff5555");
    }
  };

  startLoop(tick, render);
});
