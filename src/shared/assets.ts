// Typed access to the generated asset manifest (src/assets-manifest.json),
// produced by `npm run assets`. PNGs live under /assets (Vite public dir).
import manifestJson from "../assets-manifest.json";

export interface Rect {
  x: number;
  y: number;
  w: number;
  h: number;
}

export interface AssetEntry {
  file: string;
  w: number;
  h: number;
  screen?: Rect;
  font?: { cellW: number; cellH: number };
  frames?: { w: number; h: number; count: number };
  /** ILBM CMAP triplets (8-bit RGB) for palette-indexed runtime rendering. */
  palette?: number[][];
}

export interface Manifest {
  assets: Record<string, AssetEntry>;
}

export const manifest = manifestJson as Manifest;

export function asset(name: string): AssetEntry {
  const e = manifest.assets[name];
  if (!e) throw new Error(`unknown asset: ${name}`);
  return e;
}

export function loadImage(name: string): Promise<HTMLImageElement> {
  const e = asset(name);
  return new Promise((resolve, reject) => {
    const img = new Image();
    img.onload = () => resolve(img);
    img.onerror = () => reject(new Error(`failed to load ${e.file}`));
    // base "./" in vite.config → assets resolve relative to the page.
    img.src = `./${e.file}`;
  });
}
