// Loads a frame-sheet asset (see scripts/build-assets.mjs) and alpha-punches
// the black corner colour transparent, same technique as BitmapFont in
// src/shared/font.ts.
import { asset, loadImage } from "../../shared/assets";

export class SpriteBank {
  readonly frameW: number;
  readonly frameH: number;
  readonly count: number;
  private sheet: HTMLCanvasElement;

  private constructor(sheet: HTMLCanvasElement, frameW: number, frameH: number, count: number) {
    this.sheet = sheet;
    this.frameW = frameW;
    this.frameH = frameH;
    this.count = count;
  }

  static async load(name: string): Promise<SpriteBank> {
    const img = await loadImage(name);
    const e = asset(name);
    const f = e.frames;
    if (!f) throw new Error(`asset ${name} has no frame metadata`);
    const c = document.createElement("canvas");
    c.width = e.w;
    c.height = e.h;
    const ctx = c.getContext("2d")!;
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(img, 0, 0);
    const data = ctx.getImageData(0, 0, e.w, e.h);
    const p = data.data;
    for (let i = 0; i < p.length; i += 4) {
      if (p[i] === 0 && p[i + 1] === 0 && p[i + 2] === 0) p[i + 3] = 0; // black key -> transparent
    }
    ctx.putImageData(data, 0, 0);
    return new SpriteBank(c, f.w, f.h, f.count);
  }

  draw(ctx: CanvasRenderingContext2D, frame: number, x: number, y: number): void {
    const i = ((frame % this.count) + this.count) % this.count;
    ctx.drawImage(this.sheet, i * this.frameW, 0, this.frameW, this.frameH, x, y, this.frameW, this.frameH);
  }
}
