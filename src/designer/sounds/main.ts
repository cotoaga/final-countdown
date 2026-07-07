// Sounds Panel — designer tool for auditioning the decoded 1990 audio bank
// (21 sfx + 4 syssfx) against its manifest metadata: do_sound call sites,
// Paula period/rate, priority. Via negativa: no Paula emulation, no
// in-browser MOD playback — native <audio> on the pre-rendered WAVs only.
// Data comes entirely from /assets/audio/manifest.json, fetched at runtime.
import { manifestToRows, sortRows, type Manifest, type Row, type SortKey } from "./rows";

const tbody = document.getElementById("sounds-body")!;
const readout = document.getElementById("readout")!;
const headers = document.querySelectorAll<HTMLTableCellElement>("th[data-key]");

const player = new Audio();
let playingBtn: HTMLButtonElement | null = null;

function stopPlaying(): void {
  if (playingBtn) playingBtn.textContent = "▶";
  playingBtn = null;
}
player.addEventListener("ended", stopPlaying);

function togglePlay(url: string, btn: HTMLButtonElement): void {
  if (playingBtn === btn) {
    player.pause();
    stopPlaying();
    return;
  }
  stopPlaying();
  player.src = url;
  void player.play();
  btn.textContent = "■";
  playingBtn = btn;
}

function renderRow(r: Row): HTMLTableRowElement {
  const tr = document.createElement("tr");
  if (r.orphan) tr.className = "orphan";

  const id = document.createElement("td");
  id.textContent = `${r.bank} ${String(r.index).padStart(2, "0")}`;
  tr.appendChild(id);

  const playCell = document.createElement("td");
  const btn = document.createElement("button");
  btn.className = "btn-mini";
  btn.textContent = "▶";
  btn.addEventListener("click", () => togglePlay(r.url, btn));
  playCell.appendChild(btn);
  tr.appendChild(playCell);

  const cells = [
    `${r.durationSec.toFixed(2)}S`,
    `${r.sampleRateHz} HZ`,
    String(r.paulaPeriod),
    String(r.volume),
    String(r.priority),
    r.orphan ? `${r.usage} · ORPHAN — NO CALL SITE` : r.usage,
  ];
  for (const text of cells) {
    const td = document.createElement("td");
    td.textContent = text;
    tr.appendChild(td);
  }
  return tr;
}

function render(rows: Row[]): void {
  tbody.replaceChildren(...rows.map(renderRow));
}

// null key = manifest order — see rows.ts for why that's the safe default.
let sortKey: SortKey | null = null;
let sortAsc = true;

async function main(): Promise<void> {
  const res = await fetch("./assets/audio/manifest.json");
  if (!res.ok) throw new Error(`manifest fetch failed: ${res.status}`);
  const manifest = (await res.json()) as Manifest;

  const rows = manifestToRows(manifest);
  const orphanCount = rows.filter((r) => r.orphan).length;
  readout.textContent = `${rows.length} SAMPLES · ${orphanCount} ORPHANED (NO C CALL SITE) · CLICK A COLUMN TO SORT`;
  render(sortRows(rows, sortKey, sortAsc));

  headers.forEach((th) => {
    th.addEventListener("click", () => {
      const key = th.dataset.key as SortKey;
      sortAsc = sortKey === key ? !sortAsc : true;
      sortKey = key;
      render(sortRows(rows, sortKey, sortAsc));
    });
  });
}

void main().catch((err) => {
  readout.textContent = `ERROR: ${err instanceof Error ? err.message : String(err)}`;
});
