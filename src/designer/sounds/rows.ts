// Pure row-shaping/sorting for the sounds panel, split out from main.ts so
// it's testable without a DOM — see test/sounds.test.ts.
export interface SoundEntry {
  index: number;
  file: string;
  paulaPeriod: number;
  sampleRateHz: number;
  volume: number;
  priority: number;
  durationSec: number;
  usage: string;
}
export interface Manifest {
  sfx: SoundEntry[];
  syssfx: SoundEntry[];
}
export type Bank = "SFX" | "SYS";
export type SortKey = "index" | "durationSec" | "sampleRateHz" | "paulaPeriod" | "volume" | "priority";

export interface Row extends SoundEntry {
  bank: Bank;
  url: string;
  orphan: boolean;
}

const ORPHAN_MARK = "no call site found in C";

export function toRow(bank: Bank, dir: string, e: SoundEntry): Row {
  return { ...e, bank, url: `./assets/audio/${dir}/${e.file}`, orphan: e.usage.includes(ORPHAN_MARK) };
}

export function manifestToRows(manifest: Manifest): Row[] {
  return [...manifest.sfx.map((e) => toRow("SFX", "sfx", e)), ...manifest.syssfx.map((e) => toRow("SYS", "syssfx", e))];
}

// null key = manifest order (sfx block, then syssfx block). sfx/sys share
// independent 0..N index ranges, so sorting by raw index by default would
// interleave the two banks.
export function sortRows(rows: Row[], key: SortKey | null, asc: boolean): Row[] {
  if (!key) return rows;
  const sorted = [...rows].sort((a, b) => a[key] - b[key]);
  return asc ? sorted : sorted.reverse();
}
