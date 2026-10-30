// Deterministic ILBM -> PNG asset pipeline.
//   ilbmtoppm -adjustcolors <raw> | pnmtopng   (netpbm, the proven decode)
// Emits public/assets/*.png and src/assets-manifest.json.
// Re-running with unchanged inputs must be diff-clean (no timestamps written).
import { spawnSync } from "node:child_process";
import { mkdirSync, writeFileSync, readFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const RAW = resolve(ROOT, "assets/raw");
const OUT = resolve(ROOT, "public/assets");
const MANIFEST = resolve(ROOT, "src/assets-manifest.json");

// name -> { raw: path under assets/raw, screen?: detect CRT rect }
// or { frames: { dir, prefix, count } } for a numbered-frame ILBM bank,
// composed into one horizontal sheet (bottom-aligned, black-key transparent
// at runtime — see src/designer/flight/sprites.ts).
const ASSETS = [
  { name: "demonlogo", raw: "pics/demonlogo" },
  { name: "titlescreen", raw: "pics/Titelscreen" },
  { name: "background", raw: "pics/Background" },
  { name: "asteroid", raw: "pics/Asteroid", palette: true },
  // BMHD: raster 640x192, page 640x256 — the Amiga screen mode stretched
  // these 192 lines to fill a 256-line 4:3 display. Rendered 1:1 they look
  // squashed into extreme landscape; stretchToPageHeight redoes that stretch.
  { name: "computer", raw: "pics/Computer", screen: true, stretchToPageHeight: true },
  { name: "computer-tercom", raw: "pics/ComputerTercom", screen: true, stretchToPageHeight: true },
  { name: "font", raw: "obj/font", font: { cellW: 8, cellH: 8 } },
  { name: "energy", raw: "obj/energy" },
  // Laira walk/air banks. Direction mapping CORRECTED 2026-07-05: the raw `a`
  // frames face LEFT (designer-verified on screen against move.c's velx>0 →
  // girlgr logic — the old a→right assignment in move-physics.md was an
  // alphabetical guess). a→left, b→right; e→left, f→right.
  { name: "girl-walk-r", frames: { dir: "girl", prefix: "b", count: 16 } },
  { name: "girl-walk-l", frames: { dir: "girl", prefix: "a", count: 16 } },
  { name: "girl-air-r", frames: { dir: "girl", prefix: "f", count: 4 } },
  { name: "girl-air-l", frames: { dir: "girl", prefix: "e", count: 4 } },
  // Ground start/stop lean banks (girlgsr/gsl, move.c set_bob:484-488) and
  // air start/end banks (girlasr/asl, set_bob:490-495). FACING IS LAWLESS:
  // the letter prefixes carry NO left/right convention (a=L b=R, c=R d=L,
  // e=L f=R, h=R i=L — all anchor-verified against a00/designer eyes,
  // 2026-07-06). Never assign a side without anchor comparison.
  { name: "girl-start-r", frames: { dir: "girl", prefix: "c", count: 8 } },
  { name: "girl-start-l", frames: { dir: "girl", prefix: "d", count: 8 } },
  { name: "girl-sair-r", frames: { dir: "girl", prefix: "h", count: 8 } },
  { name: "girl-sair-l", frames: { dir: "girl", prefix: "i", count: 8 } },
  // girltr — rotate-toward-camera bank (use-device/hide/lift, move.c:361,
  // set_bob:496-517). Dev source `g00..g15`: g00 is dead-on front-facing,
  // g15 is full side profile — visually confirmed 2026-07-08. The shipped
  // game halved this to girltr[8] (girlshp>>2); this sandbox keeps all 16
  // (girlshp>>1) for a smoother preview at the same total tick duration.
  { name: "girl-turn", frames: { dir: "girl", prefix: "g", count: 16 } },
  // girle[21] — die AND appear (set_bob SMDIE/SMAPP, move.c; girlshp caps at
  // 81 → 21 frames @ >>2). Raw group `j` per reference/move-physics.md.
  { name: "girl-die", frames: { dir: "girl", prefix: "j", count: 21 } },
];

// Extract the ILBM CMAP chunk (raw palette triplets, 8-bit as stored).
// Needed where runtime rendering is palette-indexed: Amiga colour cycling
// and index-0 transparency (transparency is INDEX 0, not "black").
function parseCMAP(path) {
  const buf = readFileSync(path);
  if (buf.toString("ascii", 0, 4) !== "FORM") throw new Error(`${path}: not IFF`);
  let i = 12;
  while (i + 8 <= buf.length) {
    const id = buf.toString("ascii", i, i + 4);
    const len = buf.readUInt32BE(i + 4);
    if (id === "CMAP") {
      const colors = [];
      for (let c = 0; c < Math.floor(len / 3); c++) {
        colors.push([buf[i + 8 + c * 3], buf[i + 8 + c * 3 + 1], buf[i + 8 + c * 3 + 2]]);
      }
      return colors;
    }
    i += 8 + len + (len & 1); // chunks are word-aligned
  }
  throw new Error(`${path}: no CMAP chunk`);
}

// BMHD: raster w/h plus the ILBM's intended "page" (screen mode) w/h.
function parseBMHD(path) {
  const buf = readFileSync(path);
  if (buf.toString("ascii", 0, 4) !== "FORM") throw new Error(`${path}: not IFF`);
  let i = 12;
  while (i + 8 <= buf.length) {
    const id = buf.toString("ascii", i, i + 4);
    const len = buf.readUInt32BE(i + 4);
    if (id === "BMHD") {
      const body = i + 8;
      return { pageW: buf.readInt16BE(body + 16), pageH: buf.readInt16BE(body + 18) };
    }
    i += 8 + len + (len & 1);
  }
  throw new Error(`${path}: no BMHD chunk`);
}

function run(cmd, args, input) {
  const r = spawnSync(cmd, args, { input, maxBuffer: 256 * 1024 * 1024 });
  if (r.status !== 0) {
    throw new Error(`${cmd} failed (${r.status}): ${r.stderr?.toString() ?? ""}`);
  }
  return r.stdout;
}

// Parse a binary P6 PPM into { w, h, max, data:Uint8Array(rgb) }.
function parsePPM(buf) {
  let i = 0;
  const tok = () => {
    while (i < buf.length && (buf[i] === 0x20 || buf[i] === 0x09 || buf[i] === 0x0a || buf[i] === 0x0d)) i++;
    if (buf[i] === 0x23) { while (i < buf.length && buf[i] !== 0x0a) i++; return tok(); } // comment
    let s = i;
    while (i < buf.length && buf[i] > 0x20) i++;
    return buf.toString("ascii", s, i);
  };
  if (tok() !== "P6") throw new Error("not a P6 ppm");
  const w = +tok(), h = +tok(), max = +tok();
  i++; // single whitespace after maxval
  return { w, h, max, data: buf.subarray(i, i + w * h * 3) };
}

// Bounding box of the main CRT: the largest connected mass of navy
// (blue-dominant, dark) pixels — ignores the smaller secondary screens.
function detectScreen({ w, h, data }) {
  const isBlue = (x, y) => {
    const p = (y * w + x) * 3;
    const r = data[p], g = data[p + 1], b = data[p + 2];
    return b > 40 && b < 200 && b > r + 30 && b > g + 30 && r < 110 && g < 110;
  };
  const seen = new Uint8Array(w * h);
  let best = null, bestArea = 0;
  const stack = [];
  for (let sy = 0; sy < h; sy++) {
    for (let sx = 0; sx < w; sx++) {
      if (seen[sy * w + sx] || !isBlue(sx, sy)) continue;
      let minX = sx, minY = sy, maxX = sx, maxY = sy, area = 0;
      stack.push(sx, sy);
      seen[sy * w + sx] = 1;
      while (stack.length) {
        const y = stack.pop(), x = stack.pop();
        area++;
        if (x < minX) minX = x;
        if (y < minY) minY = y;
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
        for (const [dx, dy] of [[1, 0], [-1, 0], [0, 1], [0, -1]]) {
          const nx = x + dx, ny = y + dy;
          if (nx >= 0 && ny >= 0 && nx < w && ny < h && !seen[ny * w + nx] && isBlue(nx, ny)) {
            seen[ny * w + nx] = 1;
            stack.push(nx, ny);
          }
        }
      }
      if (area > bestArea) { bestArea = area; best = { minX, minY, maxX, maxY }; }
    }
  }
  if (!best) return null;
  // Inset 2px so text sits inside the phosphor, not on the bezel edge.
  return { x: best.minX + 2, y: best.minY + 2, w: best.maxX - best.minX - 3, h: best.maxY - best.minY - 3 };
}

// Nearest-neighbour vertical stretch — keeps pixel edges crisp (matches the
// `image-rendering: pixelated` convention) instead of blurring with interpolation.
function stretchVertical({ w, h, data }, targetH) {
  if (h === targetH) return { w, h, data };
  const out = Buffer.alloc(w * targetH * 3);
  for (let y = 0; y < targetH; y++) {
    const sy = Math.floor((y * h) / targetH);
    data.copy(out, y * w * 3, sy * w * 3, (sy + 1) * w * 3);
  }
  return { w, h: targetH, data: out };
}

function ppmBuffer(w, h, data) {
  return Buffer.concat([Buffer.from(`P6\n${w} ${h}\n255\n`, "ascii"), data]);
}

// Compose numbered frames into one horizontal sheet. Frames are trimmed to
// varying sizes by ilbmtoppm; pad to the bank's max w/h, bottom-aligned so
// the feet baseline stays put (the height wobble IS the walk bob — keep it).
function composeFrames(frames) {
  const frameW = Math.max(...frames.map((f) => f.w));
  const frameH = Math.max(...frames.map((f) => f.h));
  const sheetW = frameW * frames.length;
  const data = Buffer.alloc(sheetW * frameH * 3, 0); // 0 = black = transparent key
  frames.forEach((f, i) => {
    const x0 = i * frameW;
    const y0 = frameH - f.h;
    for (let y = 0; y < f.h; y++) {
      const srcOff = y * f.w * 3;
      const dstOff = ((y0 + y) * sheetW + x0) * 3;
      f.data.copy(data, dstOff, srcOff, srcOff + f.w * 3);
    }
  });
  return { w: sheetW, h: frameH, frameW, frameH, data };
}

mkdirSync(OUT, { recursive: true });
mkdirSync(dirname(MANIFEST), { recursive: true });
const manifest = { assets: {} };

for (const a of ASSETS) {
  let composed;
  if (a.frames) {
    const { dir, prefix, count } = a.frames;
    const frames = [];
    for (let i = 0; i < count; i++) {
      const idx = String(i).padStart(2, "0");
      const buf = run("ilbmtoppm", ["-adjustcolors", resolve(RAW, dir, `${prefix}${idx}`)]);
      frames.push(parsePPM(buf));
    }
    composed = composeFrames(frames);
  } else {
    const buf = run("ilbmtoppm", ["-adjustcolors", resolve(RAW, a.raw)]);
    const p = parsePPM(buf);
    composed = { w: p.w, h: p.h, data: p.data };
    if (a.stretchToPageHeight) {
      const { pageH } = parseBMHD(resolve(RAW, a.raw));
      composed = stretchVertical(composed, pageH);
    }
  }

  const png = run("pnmtopng", [], ppmBuffer(composed.w, composed.h, composed.data));
  const file = `assets/${a.name}.png`;
  writeFileSync(resolve(OUT, `${a.name}.png`), png);

  const entry = { file, w: composed.w, h: composed.h };
  if (a.screen) {
    const s = detectScreen(composed);
    if (!s) throw new Error(`no screen rect detected in ${a.name}`);
    entry.screen = s;
  }
  if (a.font) entry.font = a.font;
  if (a.palette) entry.palette = parseCMAP(resolve(RAW, a.raw));
  if (a.frames) entry.frames = { w: composed.frameW, h: composed.frameH, count: a.frames.count };
  manifest.assets[a.name] = entry;
  console.log(
    `${a.name.padEnd(16)} ${composed.w}x${composed.h}` +
      `${entry.screen ? `  screen ${JSON.stringify(entry.screen)}` : ""}` +
      `${entry.frames ? `  frames ${JSON.stringify(entry.frames)}` : ""}`,
  );
}

// Stable key order so the manifest is diff-clean across runs.
const ordered = { assets: {} };
for (const k of Object.keys(manifest.assets).sort()) ordered.assets[k] = manifest.assets[k];
const json = JSON.stringify(ordered, null, 2) + "\n";
let prev = "";
try { prev = readFileSync(MANIFEST, "utf8"); } catch { /* first run */ }
if (prev !== json) writeFileSync(MANIFEST, json);
console.log(`\nmanifest -> ${MANIFEST}${prev === json ? " (unchanged)" : ""}`);
