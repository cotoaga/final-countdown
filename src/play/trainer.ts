// In-page trainer for the play slice (COT-253) — the browser's answer to the
// original's trainer ADF. Shift+D toggles. One `flags` object (move.ts), one
// checkbox list, one <pre> readout. State round-trips through the URL query so
// a prod link is a repro. Pure helpers are exported for tests; DOM lives only
// inside mountTrainer().
import type { DeckMatrix } from '../levels/decoder';
import { matrixCell } from '../levels/decoder';
import { DEFAULT_FLAGS, MENERGY, MGROUND, type Laira, type PlayFlags, type PlayInput, mxpOf, tickLaira } from './move';

type BoolFlag = Exclude<keyof PlayFlags, 'devices'>;

// Rows are [flag, label]. Checked == flag true == canon behaviour for kill/fuel
// switches; for `noclip` checked == ground ignored.
const SWITCHES: Array<[BoolFlag, string]> = [
  ['fuelBurn', 'fuel burn (grav_sim fuel--)'],
  ['energyKills', 'energy fields MENERGY 0x4000'],
  ['mines', 'mines MMINE (M2, unwired)'],
  ['trapKills', 'traps MTRAP'],
  ['fallKills', 'fall death'],
  ['foeKills', 'foes contact (M5, unwired)'],
  ['noclip', 'noclip (MGROUND solidity)'],
  ['gravity', 'gravity'],
  ['liftTimers', 'lift timers LIFTLIFE 65 (M2, unwired)'],
];
// GOD = every cause of death plus fuel burn off — the original's `master` split per cause.
const GOD_OFF: BoolFlag[] = ['fuelBurn', 'energyKills', 'mines', 'trapKills', 'fallKills', 'foeKills'];

// elements.ts: bits 9–10 are unaccounted-for — painted loudly, never guessed.
export const MUNKNOWN = 0x0600;
export const MITEM = 0x3800;

export interface Probes {
  tiles: boolean;
  box: boolean;
}

// The ticket's `?fuel=0` reads as fuelBurn off; the canonical key stays fuelBurn.
const ALIASES: Record<string, BoolFlag> = { fuel: 'fuelBurn' };
const PROBE_KEYS: Array<keyof Probes> = ['tiles', 'box'];

export function parseQuery(search: string): { flags: PlayFlags; probes: Probes; any: boolean } {
  const q = new URLSearchParams(search);
  const flags: PlayFlags = { ...DEFAULT_FLAGS };
  const probes: Probes = { tiles: false, box: false };
  if (q.get('god') === '1') for (const k of GOD_OFF) flags[k] = false;
  const bit = (v: string | null) => (v === '0' || v === '1' ? v === '1' : null);
  for (const [k] of SWITCHES) {
    const v = bit(q.get(k));
    if (v !== null) flags[k] = v;
  }
  for (const [a, k] of Object.entries(ALIASES)) {
    const v = bit(q.get(a));
    if (v !== null) flags[k] = v;
  }
  for (const k of PROBE_KEYS) probes[k] = q.get(k) === '1';
  const dev = q.get('dev');
  if (dev && /^[0-9a-f]{1,8}$/i.test(dev)) flags.devices = parseInt(dev, 16) >>> 0;
  return { flags, probes, any: [...q.keys()].length > 0 };
}

/** Only non-default state is written, so a clean URL means canon. */
export function toQuery(flags: PlayFlags, probes: Probes): string {
  const q = new URLSearchParams();
  for (const [k] of SWITCHES) if (flags[k] !== DEFAULT_FLAGS[k]) q.set(k, flags[k] ? '1' : '0');
  if (flags.devices) q.set('dev', (flags.devices >>> 0).toString(16));
  for (const k of PROBE_KEYS) if (probes[k]) q.set(k, '1');
  return q.toString();
}

/** Stick stream as [joyx, thrust, run-length] triples. */
export type InputRuns = Array<[-1 | 0 | 1, 0 | 1, number]>;

export function encodeInputs(inputs: PlayInput[]): InputRuns {
  const runs: InputRuns = [];
  for (const i of inputs) {
    const t = i.thrust ? 1 : 0;
    const last = runs[runs.length - 1];
    if (last && last[0] === i.joyx && last[1] === t) last[2]++;
    else runs.push([i.joyx, t, 1]);
  }
  return runs;
}

export function decodeInputs(runs: InputRuns): PlayInput[] {
  return runs.flatMap(([joyx, t, n]) => Array.from({ length: n }, () => ({ joyx, thrust: t === 1 })));
}

/** Feet cell: row+3, body column col+1 (mv.asm check_ground). */
export const feetCell = (l: Laira) => ({ row: (l.yp >> 10) + 3, col: (mxpOf(l) >> 9) + 1 });

export interface Fixture {
  flags: PlayFlags;
  start: Laira;
  inputs: InputRuns;
  end: { row: number; col: number; xp: number; yp: number; tick: number };
}

export const endOf = (l: Laira): Fixture['end'] => ({ ...feetCell(l), xp: l.xp, yp: l.yp, tick: l.tick });

/** Deterministic replay: same start, flags and stick stream → same end. */
export function replay(f: Fixture, m: DeckMatrix): Fixture['end'] {
  let l = structuredClone(f.start);
  for (const i of decodeInputs(f.inputs)) l = tickLaira(l, m, i, f.flags);
  return endOf(l);
}

export interface TrainerHost {
  flags: PlayFlags;
  matrix: DeckMatrix;
  canvas: HTMLCanvasElement;
  playH: number; // height of the playfield above the status bar
  anchor: { dx: number; dy: number }; // bob centre / feet baseline offsets from (xp>>5, yp>>6)
  get(): Laira;
  set(l: Laira): void;
}

export interface Trainer {
  /** Ask once per loop tick: should the sim advance? (pause / step / 10 Hz) */
  gate(): boolean;
  /** Stick for this tick — live, recorded, or replayed. */
  input(raw: PlayInput): PlayInput;
  /** Call after tickLaira ran. */
  afterTick(): void;
  /** Draw probes over the playfield and refresh the readout. */
  draw(ctx: CanvasRenderingContext2D): void;
}

const el = <K extends keyof HTMLElementTagNameMap>(tag: K, text = ''): HTMLElementTagNameMap[K] => {
  const e = document.createElement(tag);
  e.textContent = text;
  return e;
};

export function mountTrainer(h: TrainerHost, initial: Probes, open: boolean): Trainer {
  const { flags } = h;
  const probes = { ...initial };
  let paused = false;
  let slow = false;
  let steps = 0;
  let beat = 0;
  let teleport = false;
  let mode: 'idle' | 'rec' | 'play' = 'idle';
  let buf: PlayInput[] = [];
  let startState: Laira | null = null;
  let startFlags: PlayFlags | null = null;
  let fixture: Fixture | null = null;
  let status = '';
  let cursor = 0;
  const refresh: Array<() => void> = [];

  const sync = () => history.replaceState(null, '', location.pathname + (toQuery(flags, probes) ? `?${toQuery(flags, probes)}` : ''));
  const check = (label: string, get: () => boolean, set: (v: boolean) => void, title = '') => {
    const l = el('label');
    const i = el('input');
    i.type = 'checkbox';
    i.checked = get();
    i.onchange = () => (set(i.checked), sync());
    refresh.push(() => (i.checked = get()));
    l.title = title;
    l.append(i, label);
    return l;
  };
  const button = (label: string, fn: () => void) => Object.assign(el('button', label), { onclick: fn });

  const panel = el('div');
  panel.className = 'trainer';
  panel.hidden = !open;
  const readout = el('pre');
  const note = el('div');
  panel.append(el('b', 'TRAINER  (Shift+D)'));
  for (const [k, label] of SWITCHES) panel.append(check(label, () => flags[k], (v) => (flags[k] = v)));
  const god = button('GOD', () => {
    for (const k of GOD_OFF) flags[k] = false;
    refresh.forEach((r) => r());
    sync();
  });
  panel.append(god, button('CANON', () => (Object.assign(flags, DEFAULT_FLAGS), refresh.forEach((r) => r()), sync())));
  const devs = el('div', 'devices 0–31');
  devs.className = 'devs';
  for (let n = 0; n < 32; n++) {
    devs.append(check(String(n), () => (flags.devices >>> n & 1) === 1, (v) => (flags.devices = (v ? flags.devices | (1 << n) : flags.devices & ~(1 << n)) >>> 0), `dev ${n}`));
  }
  panel.append(devs, el('b', 'PROBES'));
  panel.append(check('tile bits (blue GROUND · yellow ENERGY · green item · MAGENTA 0x0600)', () => probes.tiles, (v) => (probes.tiles = v)));
  panel.append(check('collision box + feet baseline', () => probes.box, (v) => (probes.box = v)));
  panel.append(check('click-to-teleport', () => teleport, (v) => (teleport = v)));
  panel.append(
    check('pause', () => paused, (v) => (paused = v)),
    check('10 Hz slow-mo', () => slow, (v) => (slow = v)),
    button('step', () => steps++),
  );
  const rec = button('REC', () => {
    if (mode === 'rec') {
      mode = 'idle';
      fixture = { flags: startFlags!, start: startState!, inputs: encodeInputs(buf), end: endOf(h.get()) };
      status = `recorded ${buf.length} ticks → feet cell ${fixture.end.row},${fixture.end.col}`;
    } else {
      mode = 'rec';
      buf = [];
      startState = structuredClone(h.get());
      startFlags = { ...flags };
      status = 'recording…';
    }
    rec.textContent = mode === 'rec' ? 'STOP' : 'REC';
  });
  const play = button('REPLAY', () => {
    if (!fixture || mode === 'rec') return;
    Object.assign(flags, fixture.flags);
    refresh.forEach((r) => r());
    h.set(structuredClone(fixture.start));
    buf = decodeInputs(fixture.inputs);
    cursor = 0;
    mode = 'play';
    status = 'replaying…';
  });
  const save = button('EXPORT', () => {
    if (!fixture) return;
    const a = Object.assign(el('a'), {
      href: URL.createObjectURL(new Blob([JSON.stringify(fixture)], { type: 'application/json' })),
      download: `fc-fixture-${fixture.end.tick}.json`,
    });
    a.click();
    URL.revokeObjectURL(a.href);
  });
  panel.append(el('b', 'INPUT RECORDER'), rec, play, save, note, readout);
  document.body.append(panel);

  window.addEventListener(
    'keydown',
    (e) => {
      if (e.shiftKey && e.key === 'D') {
        panel.hidden = !panel.hidden;
        e.preventDefault();
        e.stopImmediatePropagation(); // 'd' is also move-right
      }
    },
    true,
  );
  h.canvas.addEventListener('click', (e) => {
    const r = h.canvas.getBoundingClientRect();
    const x = ((e.clientX - r.left) * h.canvas.width) / r.width;
    const y = ((e.clientY - r.top) * h.canvas.height) / r.height;
    if (!teleport || y >= h.playH) return;
    const yp = Math.max(0, Math.round((y - h.anchor.dy) * 64));
    h.set({ ...h.get(), xp: Math.max(0, Math.round((x - h.anchor.dx) * 32)), yp, velx: 0, vely: 0 });
  });

  const outline = (ctx: CanvasRenderingContext2D, xpos: number, row: number, col: number, colour: string) => {
    ctx.strokeStyle = colour;
    ctx.strokeRect(col * 16 - xpos + 0.5, row * 16 + 0.5, 15, 15);
  };

  return {
    gate() {
      if (steps > 0) return steps--, true;
      if (paused) return false;
      return !slow || beat++ % 5 === 0; // 50 Hz / 5 = 10 Hz
    },
    input(raw) {
      if (mode === 'rec') buf.push(raw);
      if (mode === 'play') return buf[cursor++] ?? { joyx: 0, thrust: false };
      return raw;
    },
    afterTick() {
      if (mode !== 'play' || cursor < buf.length) return;
      mode = 'idle';
      const got = endOf(h.get());
      const want = fixture!.end;
      const same = got.row === want.row && got.col === want.col && got.xp === want.xp && got.yp === want.yp;
      status = same ? `REPLAY MATCH — feet cell ${got.row},${got.col}` : `REPLAY DIVERGED — got ${got.row},${got.col} want ${want.row},${want.col}`;
    },
    draw(ctx) {
      const l = h.get();
      const xpos = l.spos >> 5;
      const c0 = xpos >> 4;
      const m = h.matrix;
      if (probes.tiles) {
        for (let r = 0; r < m.rows; r++) {
          for (let c = c0; c <= c0 + h.canvas.width / 16 && c < m.cols; c++) {
            const v = matrixCell(m, r, c);
            const paint = (on: number, colour: string) => {
              if (!on) return;
              ctx.fillStyle = colour;
              ctx.fillRect(c * 16 - xpos, r * 16, 16, 16);
            };
            paint(v & MGROUND, 'rgba(60,120,255,.35)');
            paint(v & MENERGY, 'rgba(255,230,0,.45)');
            paint(v & MITEM, 'rgba(0,255,100,.5)');
            paint(v & MUNKNOWN, 'rgba(255,0,255,.85)');
          }
        }
      }
      if (probes.box) {
        // The sim is cell-based (mv.asm): body col+1 rows +1..+2, feet row+3,
        // walls probed at col+3 / col-1.
        const row = l.yp >> 10;
        const col = mxpOf(l) >> 9;
        outline(ctx, xpos, row + 1, col + 1, '#00ffff');
        outline(ctx, xpos, row + 2, col + 1, '#00ffff');
        outline(ctx, xpos, row + 3, col + 1, '#ff8a00');
        for (const [r, c] of [[row + 2, col + 3], [row + 2, col - 1], [row + 1, col - 1]]) outline(ctx, xpos, r, c, '#ff3030');
        const fy = (l.yp >> 6) + h.anchor.dy;
        ctx.fillStyle = '#ff8a00';
        ctx.fillRect(0, fy, h.canvas.width, 1);
      }
      if (panel.hidden) return;
      const f = feetCell(l);
      note.textContent = status;
      readout.textContent =
        `tick ${l.tick}  SM ${l.phase}/${l.ground.name}/${l.flying ? 'fly' : 'gnd'}\n` +
        `xp ${l.xp}  spos ${l.spos}  yp ${l.yp}\nvelx ${l.velx}  vely ${l.vely}  fuel ${l.fuel}\n` +
        `feet cell ${f.row},${f.col} = ${f.row < m.rows && f.col < m.cols ? '0x' + matrixCell(m, f.row, f.col).toString(16).padStart(4, '0') : 'off-map'}`;
    },
  };
}
