import { describe, expect, it } from "vitest";
import {
  AUTCODEDEV,
  AUTCODEFIL,
  DECK_COUNT,
  DISKFILES,
  PASSCOMP,
  PASSFILE,
  PCOMNO,
  devicesFor,
  inventoryFor,
  sectionOf,
} from "../src/terminal/decks";

describe("deck tables", () => {
  it("all per-deck tables have one entry per deck", () => {
    for (const t of [PCOMNO, PASSCOMP, PASSFILE, AUTCODEDEV, AUTCODEFIL, DISKFILES]) {
      expect(t).toHaveLength(DECK_COUNT);
    }
  });

  it("matches the source-ground constants", () => {
    expect(PCOMNO).toEqual([9, 10, 7, 2, 5, 1, 8, 11]);
    expect(PASSCOMP[0]).toBe("NASKUR");
    expect(PASSFILE[7]).toBe("CORMUR");
  });

  it("sections follow the deck bands from cli.c INFO", () => {
    expect(sectionOf(0)).toBe("SECTION III");
    expect(sectionOf(3)).toBe("SECTION III");
    expect(sectionOf(4)).toBe("SECTION II");
    expect(sectionOf(6)).toBe("SECTION II");
    expect(sectionOf(7)).toBe("SECTION I");
  });

  it("returns fresh device/inventory copies (no shared mutation)", () => {
    const a = devicesFor(0);
    a[0].on = !a[0].on;
    expect(devicesFor(0)[0].on).not.toBe(a[0].on);
    expect(inventoryFor(0)).not.toBe(inventoryFor(0));
  });
});
