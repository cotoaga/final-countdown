import { describe, expect, it } from "vitest";
import { manifestToRows, sortRows, type Manifest, type SoundEntry } from "../src/designer/sounds/rows";

const entry = (index: number, over: Partial<SoundEntry> = {}): SoundEntry => ({
  index,
  file: `sfx${index}.wav`,
  paulaPeriod: 856,
  sampleRateHz: 4144,
  volume: 64,
  priority: 7,
  durationSec: 1,
  usage: "some call site",
  ...over,
});

describe("manifestToRows", () => {
  const manifest: Manifest = {
    sfx: [entry(0), entry(1)],
    syssfx: [entry(0), entry(1)],
  };

  it("keeps manifest order: all sfx, then all syssfx (default view must not interleave)", () => {
    const rows = manifestToRows(manifest);
    expect(rows.map((r) => `${r.bank}${r.index}`)).toEqual(["SFX0", "SFX1", "SYS0", "SYS1"]);
  });

  it("builds a play URL under the bank's own directory", () => {
    const rows = manifestToRows(manifest);
    expect(rows[0].url).toBe("./assets/audio/sfx/sfx0.wav");
    expect(rows[2].url).toBe("./assets/audio/syssfx/sfx0.wav");
  });

  it("flags orphans by the manifest's own usage text, not a hardcoded index list", () => {
    const m: Manifest = {
      sfx: [entry(9, { usage: "(no call site found in C — likely asm or unused)" }), entry(1)],
      syssfx: [],
    };
    const rows = manifestToRows(m);
    expect(rows[0].orphan).toBe(true);
    expect(rows[1].orphan).toBe(false);
  });
});

describe("sortRows", () => {
  const rows = manifestToRows({
    sfx: [entry(0), entry(1)],
    syssfx: [entry(0), entry(1)],
  });

  it("with a null key, returns manifest order unchanged (sfx/sys share 0..N ranges)", () => {
    // Regression: sorting by raw "index" by default interleaves SFX/SYS
    // rows, since both banks number their samples independently from 0.
    expect(sortRows(rows, null, true)).toBe(rows);
  });

  it("sorts ascending by the given numeric key", () => {
    const withPriority = manifestToRows({
      sfx: [entry(0, { priority: 15 }), entry(1, { priority: 3 })],
      syssfx: [],
    });
    const sorted = sortRows(withPriority, "priority", true);
    expect(sorted.map((r) => r.priority)).toEqual([3, 15]);
  });

  it("reverses to descending", () => {
    const withPriority = manifestToRows({
      sfx: [entry(0, { priority: 15 }), entry(1, { priority: 3 })],
      syssfx: [],
    });
    const sorted = sortRows(withPriority, "priority", false);
    expect(sorted.map((r) => r.priority)).toEqual([15, 3]);
  });
});
