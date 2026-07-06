// Per-deck terminal data, ported from the original source.
//   passcomp/passfile/autcodedev/autcodefil .... reference/src/cli.c
//   pcomno (which computer is the protected one) ........ reference/src/comp.c
//   diskfiles (the 8 shared disks) ...................... reference/src/start.c
//
// KILL ORDER (COT-81, Kurt 2026-07-06): cli.c ALSO contains manual-based COPY
// PROTECTION — chklin1/2/3 (cli.c:64-78, fired at cli.c:222-224): random
// questions about the printed Kurzübersicht card / "SEITE 7" of the paper
// manual; wrong answer sets `fatal`. Deliberately NOT ported, never to be
// ported. Do not mistake it for a missing feature. The in-game access codes
// (AUTCODEDEV/PASSCOMP below) are GAMEPLAY and stay.
// The canonical per-computer SYSTEM file lists and device layouts live in the
// binary DISK01:lev/Struct0N level files, which are NOT in this repo. Device
// sets below are marked ILLUSTRATIVE: they exercise the real command logic
// (ENABLE/DISABLE/LIST/STATUS) without claiming to be the shipped layout.

export const DECK_COUNT = 8;

// comp.c: which computer number on each deck is the protected ("procom") one.
export const PCOMNO = [9, 10, 7, 2, 5, 1, 8, 11];

// cli.c: device / file access codes per deck.
export const PASSCOMP = ["NASKUR", "WPERYOD", "AXYRN", "SARDANYX", "CUTHUR", "NASKUR", "ENOBYNLEM", "KALATORYE"];
export const PASSFILE = ["DOPHAZ", "ITLARS", "DESKIDE", "F4", "F5", "F6", "F7", "CORMUR"];

// cli.c: the prompt shown when a protected device / file is accessed.
export const AUTCODEDEV = [
  "ACCESS CODE DECK 1 ?",
  "ACCESS CODE DECK 2 ?",
  "TACH.DES AUTHORISATION SYMBOL ?",
  "EKIN.MAN AUTHORISATION SYMBOL ?",
  "ENTER PERSONAL IDENTIFIKATION !",
  "ACCESS CODE DECK1 ?",
  "ENTER KEY aabbbcccc+",
  "MGARRYNN ACCESS SYMBOL ?",
];
export const AUTCODEFIL = [
  "SECTION III CODE abc ?",
  "SECTION III CODE def ?",
  "CHECK SYMBOL OF BETAH H5259 ?",
  "ENTER F4 CODE+",
  "ENTER F5 CODE+",
  "ENTER F6 CODE+",
  "ENTER F7 CODE+",
  "NAME OF NAVIGATION SYSTEM ?",
];

// start.c: the eight shared disks and their files (blank "  " entries dropped).
export const DISKFILES: string[][] = [
  ["DESOLATOR.TXT", "DERURTACHYN.PRG", "HELP.TXT", "DESCRIPTION.PIC"],
  ["ASSIMILATOR.TXT", "DERURASSIM.PRG", "HANDLING.TXT", "TECHNIK.PIC"],
  ["VICHOROFF.TXT", "DERURVIHF.PRG", "DOCUMENT.TXT", "TECHNIK.PIC", "TARYN.TXT", "DERURTRN.PRG", "HELP.TXT", "DESCRIPTION.PIC"],
  ["DERUR.PRG", "MOEBIUS.PRG", "HELP.TXT"],
  ["NASKUR.TXT", "DERURNSKR.PRG", "HANDLING.TXT", "TECHNIK.PIC"],
  ["DEIMARTRANS.TXT", "CORURDEIMAR.PRG", "DOCUMENT.TXT"],
  ["ORDER298.PRG", "ORDER096.PRG", "DESCRIPT.TXT"],
  ["CLIMATE.PRG", "CODES.TXT", "USAGE.TXT"],
];

// cli.c INFO: section by deck (0-indexed level).
export function sectionOf(level: number): string {
  if (level > 6) return "SECTION I";
  if (level > 3) return "SECTION II";
  return "SECTION III";
}

export type DeviceType = "GATE" | "ROBOT" | "LIFT";

export interface Device {
  typ: DeviceType;
  on: boolean;
  /** accessmode bitfield: 1 = may ENABLE, 2 = may DISABLE, 3 = both. */
  access: number;
}

// ILLUSTRATIVE device set per deck (see header). Enough variety to show the
// real LIST / ENABLE / DISABLE / accessmode behaviour from cli.c.
const DECK_DEVICES: Device[][] = [
  [{ typ: "GATE", on: false, access: 3 }, { typ: "LIFT", on: true, access: 2 }, { typ: "ROBOT", on: true, access: 1 }],
  [{ typ: "GATE", on: true, access: 3 }, { typ: "GATE", on: false, access: 1 }, { typ: "LIFT", on: false, access: 3 }],
  [{ typ: "ROBOT", on: true, access: 2 }, { typ: "LIFT", on: true, access: 3 }],
  [{ typ: "GATE", on: false, access: 3 }, { typ: "GATE", on: true, access: 3 }, { typ: "ROBOT", on: false, access: 1 }, { typ: "LIFT", on: true, access: 2 }],
  [{ typ: "LIFT", on: false, access: 1 }, { typ: "GATE", on: true, access: 2 }],
  [{ typ: "GATE", on: true, access: 3 }, { typ: "ROBOT", on: true, access: 3 }, { typ: "ROBOT", on: false, access: 1 }],
  [{ typ: "GATE", on: false, access: 3 }, { typ: "LIFT", on: true, access: 3 }],
  [{ typ: "ROBOT", on: true, access: 2 }, { typ: "GATE", on: true, access: 3 }, { typ: "LIFT", on: false, access: 3 }],
];

export function devicesFor(level: number): Device[] {
  return DECK_DEVICES[level].map((d) => ({ ...d }));
}

export interface Inventory {
  fuel: number;
  mines: number;
  disk: number; // bitmask of held disks (bit i => DISK i+1)
}

// ILLUSTRATIVE starting inventory per deck — gives INFO / CD DISK something
// to report. Deck 1 starts with disks 1 and 4 held, etc.
const DECK_INVENTORY: Inventory[] = [
  { fuel: 150, mines: 3, disk: 0b00001001 },
  { fuel: 200, mines: 2, disk: 0b00000011 },
  { fuel: 150, mines: 4, disk: 0b00000101 },
  { fuel: 100, mines: 1, disk: 0b00001000 },
  { fuel: 200, mines: 3, disk: 0b00010000 },
  { fuel: 100, mines: 2, disk: 0b00000010 },
  { fuel: 100, mines: 5, disk: 0b01000000 },
  { fuel: 100, mines: 2, disk: 0b10000000 },
];

export function inventoryFor(level: number): Inventory {
  return { ...DECK_INVENTORY[level] };
}
