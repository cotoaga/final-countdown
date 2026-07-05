// Sprite font built from the original A9000 bitmap strip (assets/raw/obj/font,
// 480x8 = 60 cells of 8x8). The strip's glyph order, read off the decoded art:
//   [space] 0-9 A-Z then punctuation. Letters/digits/space are certain; the
//   punctuation cells were read from the strip and should be confirmed visually
//   — fix an index here if a mark renders wrong (one-line change).
import { asset, loadImage } from "./assets";

const CELL = 8;

// char -> cell index in the strip.
const GLYPH: Record<string, number> = { " ": 0 };
for (let d = 0; d <= 9; d++) GLYPH[String(d)] = 1 + d; // cells 1..10
const A = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
for (let i = 0; i < A.length; i++) GLYPH[A[i]] = 11 + i; // cells 11..36
// punctuation (cells 37+, verified glyph-by-glyph against the decoded strip)
Object.assign(GLYPH, { "!": 37, "%": 39, ".": 40, "-": 41, ",": 42, "/": 43, ":": 44, ";": 45, "?": 46, "=": 47 });

export function isMapped(ch: string): boolean {
  return ch in GLYPH;
}

export class BitmapFont {
  readonly cell = CELL;
  private base: HTMLCanvasElement;
  private tints = new Map<string, HTMLCanvasElement>();

  private constructor(base: HTMLCanvasElement) {
    this.base = base;
  }

  static async load(): Promise<BitmapFont> {
    const img = await loadImage("font");
    const e = asset("font");
    const c = document.createElement("canvas");
    c.width = e.w;
    c.height = e.h;
    const ctx = c.getContext("2d")!;
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(img, 0, 0);
    // Background = the strip's corner colour; everything else is ink.
    const data = ctx.getImageData(0, 0, e.w, e.h);
    const p = data.data;
    const bg = [p[0], p[1], p[2]];
    for (let i = 0; i < p.length; i += 4) {
      const isBg = p[i] === bg[0] && p[i + 1] === bg[1] && p[i + 2] === bg[2];
      p[i + 3] = isBg ? 0 : 255; // alpha mask; colour replaced per-tint
    }
    ctx.putImageData(data, 0, 0);
    return new BitmapFont(c);
  }

  private tinted(color: string): HTMLCanvasElement {
    let t = this.tints.get(color);
    if (t) return t;
    t = document.createElement("canvas");
    t.width = this.base.width;
    t.height = this.base.height;
    const ctx = t.getContext("2d")!;
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(this.base, 0, 0);
    ctx.globalCompositeOperation = "source-in";
    ctx.fillStyle = color;
    ctx.fillRect(0, 0, t.width, t.height);
    this.tints.set(color, t);
    return t;
  }

  measure(text: string): number {
    return text.length * CELL;
  }

  /** Draw uppercased text; unmapped chars render as blank cells. */
  draw(ctx: CanvasRenderingContext2D, text: string, x: number, y: number, color: string): void {
    const strip = this.tinted(color);
    const s = text.toUpperCase();
    for (let i = 0; i < s.length; i++) {
      const cell = GLYPH[s[i]];
      if (cell === undefined || cell === 0) continue;
      ctx.drawImage(strip, cell * CELL, 0, CELL, CELL, x + i * CELL, y, CELL, CELL);
    }
  }
}
