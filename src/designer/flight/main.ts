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
import { initTerminalTurn, stepTerminalTurn, terminalTurnFrame, triggerTerminalTurn } from "./terminal-turn";
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

let terminalTurn = initTerminalTurn();

const keys = new Set<string>();
const keyName = (k: string) => (k === " " ? "space" : k.toLowerCase());
window.addEventListener("keydown", (e) => {
  if (e.target instanceof HTMLInputElement) return;
  const k = keyName(e.key);
  if (["arrowleft", "arrowright", "arrowup", "arrowdown", "space", "a", "d", "r"].includes(k)) e.preventDefault();
  keys.add(k);
  if (k === "r") refuel();
  // Terminal-turn preview: grounded, idle, and not already mid-lean/turn.
  // The real trigger (move.c:361-366) is stick UP + fire at a device tile;
  // this sandbox has no device tiles, so it's a direct keypress instead.
  if (k === "arrowdown" && !e.repeat) {
    const onGround = body.yp >= world.floorYp;
    if (onGround && body.velx === 0 && ground.name === "ground") {
      terminalTurn = triggerTerminalTurn(terminalTurn);
    }
  }
});
window.addEventListener("keyup", (e) => {
  if (e.target instanceof HTMLInputElement) return;
  keys.delete(keyName(e.key));
});

let anim = initAnim();
let ground = initGroundState();

guard(canvas, async () => {
  const font = await BitmapFont.load();
  const [walkR, walkL, airR, airL, startR, startL, sairR, sairL, turnBank] = await Promise.all([
    SpriteBank.load("girl-walk-r"),
    SpriteBank.load("girl-walk-l"),
    SpriteBank.load("girl-air-r"),
    SpriteBank.load("girl-air-l"),
    SpriteBank.load("girl-start-r"),
    SpriteBank.load("girl-start-l"),
    SpriteBank.load("girl-sair-r"),
    SpriteBank.load("girl-sair-l"),
    SpriteBank.load("girl-turn"),
  ]);

  syncSliderUI();

  const tick = () => {
    terminalTurn = stepTerminalTurn(terminalTurn);
    // SMTURNS/SMTURNE locks joyx = joyy = 0 (move.c:150,157) — the turn
    // preview owns the stick while it plays.
    const turning = terminalTurn.active;
    const joyx = turning
      ? 0
      : keys.has("arrowleft") || keys.has("a")
        ? -1
        : keys.has("arrowright") || keys.has("d")
          ? 1
          : 0;
    const thrustHeld = !turning && (keys.has("arrowup") || keys.has("space"));
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
    const gs = groundStateSprite(ground);
    let bank: SpriteBank;
    let frame: number;
    if (terminalTurn.active) {
      bank = turnBank;
      frame = terminalTurnFrame(terminalTurn);
    } else if (gs) {
      bank = gs.bank === "start" ? (gs.side === "r" ? startR : startL) : gs.side === "r" ? sairR : sairL;
      frame = gs.frame;
    } else if (body.velx === 0) {
      // Idle: face the player. Ground -> girltr's front pose (Kurt's call,
      // 2026-07-08 — canon's actual SMGROUND-idle was girlgsr[0], a side
      // lean, not front-facing). Air -> girlasr[0] (set_bob's SMFLY-idle
      // case, move.c:470) — faithful, not a deviation.
      bank = flying ? sairR : turnBank;
      frame = 0;
    } else {
      bank = flying ? (anim.facing === 1 ? airR : airL) : anim.facing === 1 ? walkR : walkL;
      frame = frameIndex(anim, body.velx);
    }
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
