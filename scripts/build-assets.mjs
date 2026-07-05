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
  { name: "asteroid", raw: "pics/Asteroid" },
  { name: "computer", raw: "pics/Computer", screen: true },
  { name: "computer-tercom", raw: "pics/ComputerTercom", screen: true },
  { name: "font", raw: "obj/font", font: { cellW: 8, cellH: 8 } },
  { name: "energy", raw: "obj/energy" },
  // Laira walk/air banks (reference/move-physics.md: girlgr/gl -> a/b, girlar/al -> e/f).
  { name: "girl-walk-r", frames: { dir: "girl", prefix: "a", count: 16 } },
  { name: "girl-walk-l", frames: { dir: "girl", prefix: "b", count: 16 } },
  { name: "girl-air-r", frames: { dir: "girl", prefix: "e", count: 4 } },
  { name: "girl-air-l", frames: { dir: "girl", prefix: "f", count: 4 } },
];

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
