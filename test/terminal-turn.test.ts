import { describe, expect, it } from "vitest";
import {
  initTerminalTurn,
  stepTerminalTurn,
  terminalTurnFrame,
  triggerTerminalTurn,
  type TerminalTurnState,
} from "../src/designer/flight/terminal-turn";

describe("terminal-turn — girltr preview state machine", () => {
  it("idle by default, frame 0", () => {
    const s = initTerminalTurn();
    expect(s.active).toBe(false);
    expect(terminalTurnFrame(s)).toBe(0);
  });

  it("trigger starts the turn at frame 0, opening", () => {
    const s = triggerTerminalTurn(initTerminalTurn());
    expect(s.active).toBe(true);
    expect(s.closing).toBe(false);
    expect(terminalTurnFrame(s)).toBe(0);
  });

  it("re-triggering mid-turn is a no-op (matches ignoring re-fire mid-animation)", () => {
    let s = triggerTerminalTurn(initTerminalTurn());
    s = stepTerminalTurn(s);
    s = stepTerminalTurn(s);
    const again = triggerTerminalTurn(s);
    expect(again).toBe(s);
  });

  it("plays forward 0 -> 15 (front to side), then reverses 15 -> 0 (side to front)", () => {
    let s = triggerTerminalTurn(initTerminalTurn());
    const frames: number[] = [terminalTurnFrame(s)];
    for (let t = 0; t < 64; t++) {
      s = stepTerminalTurn(s);
      frames.push(terminalTurnFrame(s));
    }
    // Reaches the full side-profile frame (15) partway through...
    expect(Math.max(...frames)).toBe(15);
    // ...and every activated tick's frame stays in [0, 15].
    expect(frames.every((f) => f >= 0 && f <= 15)).toBe(true);
    // First half strictly ascends toward 15, then it descends back toward 0.
    const peakIndex = frames.indexOf(15);
    expect(frames.slice(0, peakIndex)).toEqual([...frames.slice(0, peakIndex)].sort((a, b) => a - b));
    expect(s.active).toBe(false); // fully unwound back to idle
  });

  it("total duration is 64 ticks (32 turning + 32 turning back)", () => {
    let s: TerminalTurnState = triggerTerminalTurn(initTerminalTurn());
    let ticks = 0;
    while (s.active && ticks < 200) {
      s = stepTerminalTurn(s);
      ticks++;
    }
    expect(ticks).toBe(64); // 32 to reach shp=31 (frame 15), 32 more to unwind past shp<0
  });

  it("stepping while idle is a no-op", () => {
    const s = initTerminalTurn();
    expect(stepTerminalTurn(s)).toBe(s);
  });
});
