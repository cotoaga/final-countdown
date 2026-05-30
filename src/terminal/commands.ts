// A9000 command interpreter — ported from reference/src/cli.c (handle_system,
// check_user, change_dir) and reference/src/comp.c (use_computer boot, do_air).
// Pure and synchronous so it can be unit-tested without a DOM. The renderer
// feeds typed lines in and prints the returned lines.
//
// Faithful to the original command surface; where canonical data is missing
// from the repo (per-computer SYSTEM files, .TXT/.PIC contents) the command
// reports honestly rather than inventing game text — see NOTE markers.

import {
  AUTCODEDEV,
  DISKFILES,
  PASSCOMP,
  PCOMNO,
  devicesFor,
  inventoryFor,
  sectionOf,
  type Device,
  type DeviceType,
  type Inventory,
} from "./decks";

export interface MissionFlags {
  itime: number;
  steexc: number;
  tarpen: number;
  viccol: number;
  hygkoo: number;
  ekiman: number;
  semcom: number;
  seldes: number;
  novbom: number;
  tacdes: number;
  intal: number;
}

export interface TerminalState {
  level: number; // 0-indexed deck
  no: number; // active computer number
  master: boolean;
  procom: boolean; // this computer is the protected one
  path: number; // 0 = SYSTEM, 1..8 = disk
  keymap: "D" | "USA";
  devices: Device[];
  inv: Inventory;
  flags: MissionFlags;
  message: number; // -1 => first-contact translation notice pending
  pending: PendingAuth | null;
}

interface PendingAuth {
  prompt: string;
  password: string;
  // re-run this command verbatim once authorised
  rerun: { word1: string; word2: string; word3: string };
}

export interface CmdResult {
  lines: string[];
  /** ENDCLI / intruder lockout — caller should close the session. */
  end: boolean;
}

const ACCDEN = "ACCESS DENIED !";
const NOAB = "THIS COMPUTER IS NOT ABLE";
const TORU = "TO RUN THIS SOFTWARE !";

export function newState(level: number): TerminalState {
  const no = PCOMNO[level];
  return {
    level,
    no,
    master: false,
    procom: true, // we always boot into the deck's protected computer (pcomno)
    path: 0,
    keymap: "USA",
    devices: devicesFor(level),
    inv: inventoryFor(level),
    flags: { itime: 0, steexc: 0, tarpen: 0, viccol: 0, hygkoo: 0, ekiman: 0, semcom: 0, seldes: 0, novbom: 0, tacdes: 0, intal: 0 },
    message: -1,
    pending: null,
  };
}

// comp.c use_computer(): the boot / contact sequence.
export function boot(s: TerminalState): string[] {
  const out: string[] = ["A 9000", "PREPARING CONTACT", "..."];
  if (s.message === -1) {
    out.push(
      "BUILT UP CONTACT.",
      "UNKNOWN SYSTEM !",
      "SCANNING ALIEN SYSTEM.",
      "...",
      "SYMBOL MEMORY NOW AVAILABLE.",
      "TRANSLATION RUNNING.",
      "...",
      "FOREIGN NAMES AND TECHNICAL TERMS",
      "WILL BE REPLACED BY ARTIFICIAL",
      "GERMAN WORDS.",
    );
    s.message = 0;
  }
  out.push("SYSTEM READY.");
  return out;
}

function parse(input: string): { word1: string; word2: string; word3: string; zahl: number } {
  const parts = input.trim().toUpperCase().split(/\s+/).filter(Boolean);
  const word1 = parts[0] ?? "";
  const word2 = parts[1] ?? "";
  const word3 = parts[2] ?? "";
  let zahl = -1;
  if (/^\d{1,2}/.test(word3)) zahl = parseInt(word3.slice(0, 2), 10);
  return { word1, word2, word3, zahl };
}

// Strip the '+' prompt-control marker the original used to mean "return to the
// prompt": a bare "+" is dropped, a trailing "+" is trimmed.
function sanitize(lines: string[]): string[] {
  const out: string[] = [];
  for (const l of lines) {
    if (l === "+") continue;
    out.push(l.endsWith("+") ? l.slice(0, -1) : l);
  }
  return out;
}

export function handle(s: TerminalState, input: string): CmdResult {
  // Awaiting an access code from a prior protected command?
  if (s.pending) {
    const p = s.pending;
    s.pending = null;
    if (!s.master && input.trim().toUpperCase() !== p.password) {
      s.flags.intal = 1;
      return { lines: sanitize(["INTRUDER ALERT CLASS I", "", "10 TIME UNITS UNTIL CLASS II", "+"]), end: true };
    }
    s.procom = false;
    // authorised — re-run the original command
    const lines = ["+"];
    const r = dispatch(s, p.rerun.word1, p.rerun.word2, p.rerun.word3, zahlOf(p.rerun.word3));
    lines.push(...r.lines);
    return { lines: sanitize(lines), end: r.end };
  }

  const { word1, word2, word3, zahl } = parse(input);
  const r = dispatch(s, word1, word2, word3, zahl);
  return { lines: sanitize(r.lines), end: r.end };
}

function zahlOf(word3: string): number {
  return /^\d{1,2}/.test(word3) ? parseInt(word3.slice(0, 2), 10) : -1;
}

// check_user(): protected computer demands the deck access code first. Returns
// null to proceed now, or the prompt lines after arming the deferred re-run.
function checkUser(s: TerminalState, w1: string, w2: string, w3: string): { lines: string[] } | null {
  if (!s.procom || s.master) return null;
  s.pending = { prompt: AUTCODEDEV[s.level], password: PASSCOMP[s.level], rerun: { word1: w1, word2: w2, word3: w3 } };
  return { lines: ["PROTECTED DEVICE", AUTCODEDEV[s.level]] };
}

function dispatch(s: TerminalState, word1: string, word2: string, word3: string, zahl: number): CmdResult {
  const L: string[] = [];
  const protect = (): boolean => {
    const c = checkUser(s, word1, word2, word3);
    if (c) {
      L.push(...c.lines);
      return true; // deferred; await code
    }
    return false;
  };

  switch (word1) {
    case "":
      return { lines: L, end: false };

    case "ENABLE":
    case "DISABLE": {
      if (protect()) return { lines: L, end: false };
      return { lines: switchDevice(s, word1 === "ENABLE", word2, zahl), end: false };
    }

    case "LIST":
      return { lines: listDevices(s), end: false };

    case "STATUS": {
      if (protect()) return { lines: L, end: false };
      return { lines: statusReport(s), end: false };
    }

    case "MEM":
      return { lines: [" CHIP 184000", " FAST 0"], end: false };

    case "HELP":
      return { lines: helpText(word2), end: false };

    case "DIR":
      return { lines: dirListing(s, word2, zahl), end: false };

    case "CD": {
      if (protect()) return { lines: L, end: false };
      return { lines: changeDir(s, word2, zahl, true), end: false };
    }

    case "INFO":
      return { lines: infoReport(s), end: false };

    case "CLS":
      return { lines: ["\f"], end: false }; // \f => clear screen (renderer)

    case "TYPE": {
      if (protect()) return { lines: L, end: false };
      return { lines: typeFile(s, word2), end: false };
    }

    case "SHOW": {
      if (protect()) return { lines: L, end: false };
      return { lines: showFile(s, word2), end: false };
    }

    case "HIFI":
      if (!word2) return { lines: ["ON OR OFF ?"], end: false };
      return { lines: [], end: false };

    case "SETMAP":
      if (!word2) return { lines: [s.keymap === "D" ? "GERMAN KEYMAP" : "ENGLISH KEYMAP"], end: false };
      if (word2 === "D") { s.keymap = "D"; return { lines: ["GERMAN KEYMAP"], end: false }; }
      if (word2 === "USA") { s.keymap = "USA"; return { lines: ["ENGLISH KEYMAP"], end: false }; }
      return { lines: [], end: false };

    case "VERS":
      return { lines: ["VERSION NUMMER: 3"], end: false };

    case "ENDCLI":
      return { lines: [], end: true };

    default:
      return { lines: specialProgram(s, word1, word2, zahl), end: false };
  }
}

function switchDevice(s: TerminalState, enable: boolean, what: string, zahl: number): string[] {
  const L: string[] = [];
  const types: Record<string, DeviceType> = { GATE: "GATE", ROBOT: "ROBOT", LIFT: "LIFT" };

  if (what === "ALPHA2") {
    if (enable) { s.master = true; L.push("WELCOME MASTER"); } else s.master = false;
    return L;
  }
  if (what === "ALL") {
    const bit = enable ? 1 : 2;
    for (let i = 0; i < s.devices.length; i++) {
      const d = s.devices[i];
      if (d.access & bit && d.on !== enable) {
        d.on = enable;
        L.push(`${d.typ} ${i + 1} ${enable ? "ENABLED" : "DISABLED"}`);
      }
    }
    return L;
  }
  const typ = types[what];
  if (!typ) return ["UNKNOWN DEVICE TYPE"];
  if (zahl === -1) return [`WHICH ${typ} ?`];
  const idx = zahl - 1;
  const d = s.devices[idx];
  if (!d || d.typ !== typ) return [`NO SUCH ${typ}`];
  if (d.on === enable) return [`${typ} IS ALREADY ${enable ? "ENABLED" : "DISABLED"}`];
  if (!(d.access & (enable ? 1 : 2))) return [ACCDEN];
  d.on = enable;
  return [`${typ} ${zahl} ${enable ? "ENABLED" : "DISABLED"}`];
}

function listDevices(s: TerminalState): string[] {
  const any = s.devices.some((d) => d.access);
  if (!any) return ["NO DIRECT ACCESSED DEVICES"];
  const L = ["DEVICE STATUS:"];
  for (let i = 0; i < s.devices.length; i++) {
    const d = s.devices[i];
    if (!d.access) continue;
    const mode = d.access === 1 ? "E" : d.access === 2 ? "D" : "ED";
    L.push(` ${d.typ.padEnd(5)} ${i + 1}  ${d.on ? "ON " : "OFF"}  ${mode}`);
  }
  return L;
}

function statusReport(s: TerminalState): string[] {
  const f = s.flags;
  const L: string[] = [];
  const row = (label: string, def: string, override?: string) => L.push(` ${label.padEnd(20)}${override ?? def}`);
  L.push(" MISSION TIME        0");
  row("TARYN-PENETRATOR", "ACTIVE", f.viccol ? "DESTROYED" : f.tarpen ? "OVERHEATING" : undefined);
  row("VICHOROFF-COLLECTOR", "ACTIVE", f.viccol ? "RESERVE" : undefined);
  row("HYGOR-KOORDINATOR", "RUNNING", f.hygkoo ? "DISABLED" : undefined);
  row("EKIN-MANIPULATOR", "READY", f.ekiman ? "STOPPED" : f.steexc ? "ACCELERATING" : undefined);
  row("NASKUR-DETONATOR", "READY", f.seldes ? "ABORTED" : f.novbom ? "WORKING" : undefined);
  row("SOLAR-ASSIMILATOR", "WORKING", f.steexc || f.ekiman ? "OFF" : f.tacdes && !f.ekiman ? "MAXIMUM POWER" : undefined);
  row("TACHYONEN-DESOLATOR", "WORKING", f.tacdes ? "OFF" : undefined);
  row("SEMORCOM", "READY", f.semcom ? "OUT OF ORDER" : undefined);
  return L;
}

function helpText(word2: string): string[] {
  if (!word2) {
    return [
      "COMMAND LIST",
      "",
      " TYPE   SHOW    CLS    CD  DIR",
      " LIST   SYSTEM  STATUS",
      " ENABLE DISABLE",
      " HELP   INFO",
      " HIFI   SETMAP",
      " ENDCLI",
      "",
      "TYPE HELP FOLLOWED BY A COMMAND",
      "FOR MORE INFORMATION",
    ];
  }
  const H: Record<string, string[]> = {
    LIST: [" PRINTS A LIST OF ALL ACCESSIBLE", " DEVICES AND THEIR ACTUAL STATUS.", " YOU CAN EITHER -E ENABLE THEM", " OR -D DISABLE THEM", " OR DO -ED BOTH"],
    SYSTEM: [" DESCRIBES SYSTEM HARDWARE", " AND CONTROL TASKS"],
    STATUS: [" LISTS ACTUAL STATUS OF THE", " MOST RELEVANT SYSTEMS."],
    ENABLE: [" USAGE: -DEVICETYPE -NUMBER/ALL", " SWITCHES THE ACCESSED DEVICE ON.", " NUMBER CAN BE REPLACED BY ALL."],
    DISABLE: [" USAGE: -DEVICETYPE -NUMBER/ALL", " SWITCHES OFF ACCESSED DEVICE", " USE  ALL  FOR ALROUND ACCESS"],
    INFO: [" GIVES YOU A BRIEF INVENTORY"],
    TYPE: [" USAGE: -TEXTFILE WITHOUT EXT", " PRINTS TEXT FILES"],
    MEM: [" AVAILABLE MEMORY OF HOST COMP"],
    SHOW: [" USAGE: -PICFILE WITHOUT EXT", " DISPLAYS PICTURE FILES"],
    CLS: [" CLEARS DISPLAY"],
    CD: [" USAGE: /SYSTEM/DISK -DISKNR", " CHANGES CURRENT DIRECTORY", " OR DISPLAYS CURRENT PATH IF", " FOLLOWED BY NOTHING"],
    HIFI: [" USAGE: ON/OFF", " SWITCHES HIFI MODE", " ON OR OFF"],
    SETMAP: [" USAGE: D/USA", " SWITCHES KEYMAP"],
    DIR: [" USAGE: /SYSTEM/DISK/ -DISKNR", " LISTS CURRENT DIRECTORY", " OR DISPLAYS THE CHOOSEN PATH", " LIKE   DIR DISK 3"],
    ENDCLI: [" TERMINATES ACCESS"],
  };
  return H[word2] ?? ["NO INFORMATION AVAILABLE"];
}

function dirListing(s: TerminalState, word2: string, zahl: number): string[] {
  let restore = -1;
  if (word2) {
    restore = s.path;
    if (changeDirSilent(s, word2, zahl)) return ["DEMANDED DISK IS NOT AVAILABLE", "CHANGE TO SYSTEM FOR MORE INFORMATION"];
  }
  const L: string[] = [];
  if (s.path === 0) {
    // NOTE: per-computer SYSTEM file list lives in the binary Struct0N level
    // data, which is not in this repo.
    L.push(" [SYSTEM FILES NOT IN ARCHIVE]", " MESSAGE.TXT");
  } else {
    for (const f of DISKFILES[s.path - 1]) L.push(` ${f}`);
  }
  if (restore !== -1) s.path = restore;
  return L;
}

function changeDir(s: TerminalState, word2: string, zahl: number, echo: boolean): string[] {
  const blocked = changeDirSilent(s, word2, zahl);
  if (blocked) return ["DEMANDED DISK IS NOT AVAILABLE", "CHANGE TO SYSTEM FOR MORE INFORMATION"];
  if (!echo) return [];
  if (s.path === 0) return ["CURRENT PATH", " SYSTEM"];
  return ["CURRENT PATH", ` DISK ${s.path}`];
}

// returns true if the requested disk is unavailable (blocked)
function changeDirSilent(s: TerminalState, word2: string, zahl: number): boolean {
  if (word2 === "SYSTEM") { s.path = 0; return false; }
  if (word2 === "DISK" && zahl >= 1 && zahl <= 8) {
    if (s.master || s.inv.disk & (1 << (zahl - 1))) { s.path = zahl; return false; }
    return true;
  }
  return false;
}

function infoReport(s: TerminalState): string[] {
  const L = [`YOU ARE ON DECK ${s.level + 1}`, `IN ${sectionOf(s.level)}`];
  L.push("YOU HAVE", ` ${s.inv.fuel} UNITS OF FUEL`, ` ${s.inv.mines} MINES`);
  const held: number[] = [];
  for (let i = 0; i < 8; i++) if (s.inv.disk & (1 << i)) held.push(i + 1);
  if (held.length) L.push(`DISK ${held.join(" ")}`);
  return L;
}

function fileIn(s: TerminalState, word2: string): string | null {
  if (!word2) return null;
  const list = s.path === 0 ? null : DISKFILES[s.path - 1];
  if (!list) return null;
  for (const f of list) {
    const base = f.replace(/\.(TXT|PIC|PRG)$/i, "");
    if (base.startsWith(word2)) return f;
  }
  return null;
}

function typeFile(s: TerminalState, word2: string): string[] {
  if (!word2) return ["TYPE MUST BE FOLLOWED BY", "A FILENAME WITHOUT APPENDIX"];
  if (s.path === 0 && word2 === "MESSAGE") {
    // NOTE: canonical Msg0N text not in repo.
    return ["PLEASE WAIT.", "TRANSLATION IN PROGRESS", "", "[MESSAGE TEXT NOT IN ARCHIVE]"];
  }
  const f = fileIn(s, word2);
  if (!f) return ["FILE NOT FOUND ERROR"];
  if (!f.endsWith(".TXT")) return ["FILE TYPE ERROR"];
  // NOTE: disk .TXT contents (DISK01:TXT/Dsk*) are not in this repo.
  return ["PLEASE WAIT.", "TRANSLATION IN PROGRESS", "", `[${f} NOT IN ARCHIVE]`];
}

function showFile(s: TerminalState, word2: string): string[] {
  if (!word2) return ["SHOW MUST BE FOLLOWED BY", "A FILENAME WITHOUT APPENDIX"];
  const f = fileIn(s, word2);
  if (!f) return ["FILE NOT FOUND ERROR"];
  if (!f.endsWith(".PIC")) return ["FILE TYPE ERROR"];
  // NOTE: .PIC blueprints exist in assets/raw/blueprints but are not wired in
  // for this slice.
  return [`[${f} NOT WIRED IN]`];
}

// Deck-gated mission programs. The originals are wired to a specific deck +
// computer; from the wrong host the original prints NOAB/TORU. We recognise the
// names and honour that gate. (Full mission-flag effects are out of scope for
// the terminal slice.)
const PROGRAMS = new Set([
  "DERUR", "MOEBIUS", "CLIMATE", "ORDER298", "ORDER096", "DERURTACHYN", "DERURASSIM",
  "DERURNSKR", "DERUREKIN", "DERURVIHF", "DERURTRN", "CORURDEIMAR", "MGARRYN", "POSITION", "DESOLATOR",
]);

function specialProgram(s: TerminalState, word1: string, _word2: string, _zahl: number): string[] {
  if (!word1) return [];
  if (PROGRAMS.has(word1)) {
    if (s.master) return ["WELCOME MASTER", "[PROGRAM EFFECT OUT OF SCOPE FOR THIS SLICE]"];
    return [NOAB, TORU];
  }
  return [s.path ? "FILE NOT FOUND ERROR" : "INVALID INPUT"];
}
