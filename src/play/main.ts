// Deck 1 play page — Laira on the real Matrix0/Struct0 geometry (COT-252).
// Logic lives in ./move.ts (pure); this file is input, loading, camera, and
// pixels. Trainer switches arrive as URL flags (?noclip=1&fuelBurn=0, or
// ?god=1 for all kills + fuel burn off); the panel is ./trainer.ts (COT-253).
import { BitmapFont } from '../shared/font';
import { startLoop } from '../shared/loop';
import { guard } from '../shared/stage';
import { TILE, buildTileSheet, loadDeck } from '../levels/load';
import { TILE_INDEX_MASK } from '../levels/elements';
import { MATRIX_ROWS, matrixCell } from '../levels/decoder';
import { frameIndex } from '../designer/flight/anim';
import { groundStateSprite } from '../designer/flight/ground-states';
import { SpriteBank } from '../designer/flight/sprites';
import { FUEL_MAX, type Laira, type PlayInput, newGame, tickLaira } from './move';
import { mountTrainer, parseQuery } from './trainer';

// externals.c: WIDTH 304 visible (320 with the scroll margin), HEIGHT 192, IHEIGHT 22.
const PLAY_H = 192;
const HUD_H = 22;
const PHOSPHOR = '#cfe0ff';

// Bob anchor, derived from mv.asm: body column is col+1 of a 3-column bob
// (walls probe col-1 / col+3), so the figure centres 24 px right of bobx;
// landing fires at (yp>>6)&15 == 11 with feet on row+3, so the feet baseline
// is 37 px below the sprite top. UNVERIFIED against vAmiga — designer eyes +
// the COT-253 collision-box overlay settle it.
const BOB_CENTER_DX = 24;
const BOB_FEET_DY = 37;

const LEVEL = 0;

const canvas = document.getElementById('screen') as HTMLCanvasElement;
const ctx = canvas.getContext('2d')!;
ctx.imageSmoothingEnabled = false;

const keys = new Set<string>();
const keyName = (k: string) => (k === ' ' ? 'space' : k.toLowerCase());
window.addEventListener('keydown', (e) => {
  const k = keyName(e.key);
  if (['arrowleft', 'arrowright', 'arrowup', 'arrowdown', 'space', 'a', 'd', 'w'].includes(k)) e.preventDefault();
  keys.add(k);
});
window.addEventListener('keyup', (e) => keys.delete(keyName(e.key)));

function readInput(): PlayInput {
  const joyx: -1 | 0 | 1 =
    keys.has('arrowleft') || keys.has('a') ? -1 : keys.has('arrowright') || keys.has('d') ? 1 : 0;
  return { joyx, thrust: keys.has('arrowup') || keys.has('space') || keys.has('w') };
}

guard(canvas, async () => {
  const { flags, probes, any } = parseQuery(location.search);
  const [font, deck, walkR, walkL, airR, airL, startR, startL, sairR, sairL, die] = await Promise.all([
    BitmapFont.load(),
    loadDeck(LEVEL),
    SpriteBank.load('girl-walk-r'),
    SpriteBank.load('girl-walk-l'),
    SpriteBank.load('girl-air-r'),
    SpriteBank.load('girl-air-l'),
    SpriteBank.load('girl-start-r'),
    SpriteBank.load('girl-start-l'),
    SpriteBank.load('girl-sair-r'),
    SpriteBank.load('girl-sair-l'),
    SpriteBank.load('girl-die'),
  ]);
  const sheet = buildTileSheet(deck.elements, deck.struct.colortable);
  const tileCount = deck.elements.count;

  let laira: Laira = newGame(LEVEL);
  // JS probe surface (CLAUDE.md verification: probe constants on prod).
  Object.defineProperty(window, 'fc', { value: { get laira() { return laira; }, flags, deck }, configurable: true });

  const trainer = mountTrainer(
    {
      flags,
      matrix: deck.matrix,
      canvas,
      playH: PLAY_H,
      anchor: { dx: BOB_CENTER_DX, dy: BOB_FEET_DY },
      get: () => laira,
      set: (l) => (laira = l),
    },
    probes,
    any, // a repro link opens with its switches visible
  );

  const tick = () => {
    if (!trainer.gate()) return;
    laira = tickLaira(laira, deck.matrix, trainer.input(readInput()), flags);
    trainer.afterTick();
  };

  const render = () => {
    ctx.fillStyle = '#000';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Tiles: camera at xpos = spos>>5 px; cell 0 blits ground[0] like trans.asm.
    const xpos = laira.spos >> 5;
    const c0 = xpos >> 4;
    const dx = -(xpos & 15);
    for (let r = 0; r < MATRIX_ROWS; r++) {
      for (let i = 0; i <= canvas.width / TILE; i++) {
        const c = c0 + i;
        if (c >= deck.matrix.cols) break;
        const idx = matrixCell(deck.matrix, r, c) & TILE_INDEX_MASK;
        if (idx < tileCount) ctx.drawImage(sheet, idx * TILE, 0, TILE, TILE, dx + i * TILE, r * TILE, TILE, TILE);
      }
    }

    // Laira (set_bob).
    if (laira.visible) {
      let bank: SpriteBank;
      let frame: number;
      const gs = groundStateSprite(laira.ground);
      const flying = laira.flying === 1;
      if (laira.phase !== 'alive') {
        bank = die; // girle: SMDIE forward, SMAPP backward — both girlshp>>2
        frame = laira.shp >> 2;
      } else if (gs) {
        bank = gs.bank === 'start' ? (gs.side === 'r' ? startR : startL) : gs.side === 'r' ? sairR : sairL;
        frame = gs.frame;
      } else if (laira.velx === 0) {
        // Canon idles: SMGROUND → girlgsr[0], SMFLY → girlasr[0] (set_bob).
        bank = flying ? sairR : startR;
        frame = 0;
      } else {
        bank = flying ? (laira.anim.facing === 1 ? airR : airL) : laira.anim.facing === 1 ? walkR : walkL;
        frame = frameIndex(laira.anim, laira.velx);
      }
      const sx = (laira.xp >> 5) + BOB_CENTER_DX - (bank.frameW >> 1);
      const sy = (laira.yp >> 6) + BOB_FEET_DY - bank.frameH;
      bank.draw(ctx, frame, sx, sy);
    }

    trainer.draw(ctx);

    // Status bar (IHEIGHT 22). statr goes red while dying (main.c:306).
    const dying = laira.collision > 1;
    ctx.fillStyle = dying ? '#400000' : '#0a0a3c';
    ctx.fillRect(0, PLAY_H, canvas.width, HUD_H);
    font.draw(ctx, 'FUEL', 6, PLAY_H + 7, PHOSPHOR);
    const barX = 46;
    const barW = 120;
    ctx.strokeStyle = PHOSPHOR;
    ctx.strokeRect(barX + 0.5, PLAY_H + 7.5, barW, 8);
    ctx.fillStyle = laira.fuel > 0 ? '#ff8a00' : '#602020';
    ctx.fillRect(barX + 1, PLAY_H + 8, Math.round((barW - 2) * Math.min(1, laira.fuel / FUEL_MAX)), 7);
    font.draw(ctx, `DECK ${LEVEL + 1}`, canvas.width - 6 - font.measure('DECK 1'), PLAY_H + 7, PHOSPHOR);
  };

  startLoop(tick, render);
});
