// Ship archive: renders the extracted 1990 text corpus + decoded diagrams.
// Data: /assets/lore/texts.json (categorized), /assets/lore/pics.json + pics/*.png.

interface Texts {
  messages: Record<string, string>;
  aktTask: Record<string, string>;
  terminals: Record<string, Record<string, Record<string, string>>>;
  system: Record<string, Record<string, string>>;
  disks: Record<string, Record<string, string>>;
  other: Record<string, string>;
}

const $ = (s: string) => document.querySelector(s) as HTMLElement;

function doc(title: string, body: string): string {
  return `<details class="doc"><summary>${title}</summary><pre>${esc(body)}</pre></details>`;
}
function esc(s: string): string {
  return s.replace(/&/g, "&amp;").replace(/</g, "&lt;");
}

const MEST = [100, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500];
const MSG_LABEL: Record<string, string> = { MSGA: "Missionsbriefing (Start)", MSGB: "HYGORORDER (fatal)" };
for (let i = 0; i < 10; i++) MSG_LABEL[`MSG${i}`] = `Funkspruch · Missionszeit ${MEST[i]}`;
const AKT_LABEL: Record<string, string> = {
  Z: "AKTTASK · Ausgangslage", Y: "AKTTASK · nach Tachyon-Kill", X: "AKTTASK · nach Assimilator-Kill",
  W: "AKTTASK · nach Ekin-Stopp", V: "AKTTASK · Novabombe aktiv",
};

async function main() {
  const [texts, picsMeta] = await Promise.all([
    fetch("./assets/lore/texts.json").then((r) => r.json()) as Promise<Texts>,
    fetch("./assets/lore/pics.json").then((r) => r.json()) as Promise<{ pics: Record<string, string> }>,
  ]);

  let h = "";
  for (const [k, v] of Object.entries(texts.messages))
    h += doc(MSG_LABEL[k] ?? k, v);
  for (const k of ["Z", "Y", "X", "W", "V"])
    if (texts.aktTask[k]) h += doc(AKT_LABEL[k], texts.aktTask[k]);
  $("#msg").innerHTML = h;

  h = "";
  for (const [deck, terms] of Object.entries(texts.terminals).sort()) {
    h += `<h2>${deck.replace("deck", "DECK ")}</h2>`;
    for (const [t, files] of Object.entries(terms).sort()) {
      const sys = texts.system[deck]?.[t];
      if (sys) h += doc(`${t} · SYSTEM`, sys);
      for (const [i, body] of Object.entries(files).sort()) h += doc(`${t} · Datei ${i}`, body);
    }
  }
  $("#term").innerHTML = h;

  h = "";
  for (const [dsk, files] of Object.entries(texts.disks).sort()) {
    h += `<h2>${dsk.replace("disk", "DISKETTE ")}</h2>`;
    for (const [nm, body] of Object.entries(files).sort()) h += doc(nm, body);
  }
  $("#disk").innerHTML = h;

  h = "";
  for (const [name, label] of Object.entries(picsMeta.pics).sort())
    h += `<figure><img loading="lazy" src="./assets/lore/pics/${name}.png" alt="${label}"><figcaption>${name} — ${label}</figcaption></figure>`;
  $("#gal").innerHTML = h;
}

document.querySelectorAll<HTMLButtonElement>("#tabs button").forEach((b) =>
  b.addEventListener("click", () => {
    document.querySelectorAll("#tabs button").forEach((x) => x.classList.remove("on"));
    document.querySelectorAll("main.lore section").forEach((x) => x.classList.remove("on"));
    b.classList.add("on");
    $("#" + b.dataset.t!).classList.add("on");
  }),
);

main();
