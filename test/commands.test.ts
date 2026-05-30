import { describe, expect, it } from "vitest";
import { boot, handle, newState } from "../src/terminal/commands";
import { PASSCOMP } from "../src/terminal/decks";

// Boot into a deck and clear the protected-computer access gate so we can
// exercise the protected commands directly.
function authed(level: number) {
  const s = newState(level);
  boot(s);
  handle(s, "STATUS"); // arms the access prompt
  handle(s, PASSCOMP[level]); // correct code clears procom
  return s;
}

describe("boot", () => {
  it("greets, runs first-contact translation, then SYSTEM READY", () => {
    const s = newState(0);
    const lines = boot(s);
    expect(lines[0]).toBe("A 9000");
    expect(lines).toContain("TRANSLATION RUNNING.");
    expect(lines.at(-1)).toBe("SYSTEM READY.");
  });

  it("only shows the translation notice on first contact", () => {
    const s = newState(0);
    boot(s);
    expect(boot(s)).not.toContain("TRANSLATION RUNNING.");
  });
});

describe("universal commands", () => {
  it("HELP lists the command set", () => {
    const s = newState(0);
    expect(handle(s, "HELP").lines).toContain("COMMAND LIST");
  });

  it("VERS reports version 3", () => {
    const s = newState(0);
    expect(handle(s, "VERS").lines).toEqual(["VERSION NUMMER: 3"]);
  });

  it("INFO reports deck and section per deck", () => {
    expect(handle(newState(0), "INFO").lines).toEqual(
      expect.arrayContaining(["YOU ARE ON DECK 1", "IN SECTION III"]),
    );
    expect(handle(newState(7), "INFO").lines).toEqual(
      expect.arrayContaining(["YOU ARE ON DECK 8", "IN SECTION I"]),
    );
  });

  it("LIST is unprotected and shows device status", () => {
    const lines = handle(newState(0), "LIST").lines;
    expect(lines[0]).toBe("DEVICE STATUS:");
    expect(lines.length).toBeGreaterThan(1);
  });

  it("CLS clears via the form-feed control", () => {
    expect(handle(newState(0), "CLS").lines).toEqual(["\f"]);
  });
});

describe("protected-computer access code", () => {
  it("prompts with the deck access code, then runs the command when correct", () => {
    const s = newState(2); // deck 3
    boot(s);
    const r1 = handle(s, "STATUS");
    expect(r1.lines).toContain("PROTECTED DEVICE");
    expect(r1.lines).toContain("TACH.DES AUTHORISATION SYMBOL ?"); // autcodedev[2]
    const r2 = handle(s, PASSCOMP[2]); // "AXYRN"
    expect(r2.lines.join(" ")).toContain("TACHYONEN-DESOLATOR");
    expect(r2.end).toBe(false);
  });

  it("raises a class-I intruder alert and locks out on a wrong code", () => {
    const s = newState(0);
    boot(s);
    handle(s, "STATUS");
    const r = handle(s, "WRONGCODE");
    expect(r.lines).toContain("INTRUDER ALERT CLASS I");
    expect(r.end).toBe(true);
  });

  it("clears the gate for the rest of the session after one auth", () => {
    const s = authed(0);
    expect(handle(s, "STATUS").lines).not.toContain("PROTECTED DEVICE");
  });
});

describe("devices", () => {
  it("ENABLE turns a gate on; repeating reports already enabled", () => {
    const s = authed(0); // deck 1: device 1 = GATE (off, access ED)
    expect(handle(s, "ENABLE GATE 1").lines).toEqual(["GATE 1 ENABLED"]);
    expect(handle(s, "ENABLE GATE 1").lines).toEqual(["GATE IS ALREADY ENABLED"]);
  });

  it("respects accessmode (ENABLE-only device cannot be DISABLEd)", () => {
    const s = authed(0); // device 3 = ROBOT, access 1 (enable only), on
    expect(handle(s, "DISABLE ROBOT 3").lines).toEqual(["ACCESS DENIED !"]);
  });

  it("rejects the wrong device type at a slot", () => {
    const s = authed(0); // slot 2 is a LIFT, not a gate
    expect(handle(s, "ENABLE GATE 2").lines).toEqual(["NO SUCH GATE"]);
  });

  it("ENABLE ALPHA2 grants master", () => {
    const s = authed(0);
    expect(handle(s, "ENABLE ALPHA2").lines).toContain("WELCOME MASTER");
    expect(s.master).toBe(true);
  });
});

describe("filesystem", () => {
  it("DIR DISK n lists shared disk files without auth when held", () => {
    const s = newState(0); // deck 1 holds disks 1 and 4
    const lines = handle(s, "DIR DISK 1").lines;
    expect(lines.some((l) => l.includes("DESOLATOR.TXT"))).toBe(true);
  });

  it("CD to an unheld disk is refused", () => {
    const s = authed(0); // CD is protected; deck 1 lacks disk 2
    expect(handle(s, "CD DISK 2").lines).toContain("DEMANDED DISK IS NOT AVAILABLE");
  });

  it("CD to a held disk sets the path", () => {
    const s = authed(0);
    handle(s, "CD DISK 1");
    expect(s.path).toBe(1);
  });
});

describe("gated programs and exit", () => {
  it("a deck program run from the wrong host is refused", () => {
    const s = authed(0);
    expect(handle(s, "DERURTACHYN").lines).toEqual(["THIS COMPUTER IS NOT ABLE", "TO RUN THIS SOFTWARE !"]);
  });

  it("ENDCLI terminates the session", () => {
    expect(handle(newState(0), "ENDCLI").end).toBe(true);
  });

  it("never emits a bare '+' prompt-control line", () => {
    const s = authed(2);
    const lines = [
      ...handle(s, "STATUS").lines,
      ...handle(s, "HELP").lines,
      ...handle(s, "INFO").lines,
      ...handle(s, "LIST").lines,
    ];
    expect(lines).not.toContain("+");
  });
});
