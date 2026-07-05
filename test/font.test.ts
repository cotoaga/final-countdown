import { describe, expect, it } from "vitest";
import { isMapped } from "../src/shared/font";

// The CLI renders uppercased text; these are the glyphs the command output
// actually relies on. A miss here means a character would render blank.
const ESSENTIAL = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ.!:%/-?=";

describe("bitmap font glyph map", () => {
  it("maps every essential CLI glyph", () => {
    for (const ch of ESSENTIAL) {
      expect(isMapped(ch), `missing glyph: ${JSON.stringify(ch)}`).toBe(true);
    }
  });
});
