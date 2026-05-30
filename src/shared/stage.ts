// Small stage helpers shared by the slices: a visible fatal-error overlay so a
// thrown error or rejected asset load shows on screen instead of a dead canvas,
// and a guard() that wires it to window errors + the async entry point.
export function fatalOverlay(canvas: HTMLCanvasElement, message: string): void {
  const ctx = canvas.getContext("2d");
  if (!ctx) return;
  ctx.fillStyle = "#200";
  ctx.fillRect(0, 0, canvas.width, canvas.height);
  ctx.fillStyle = "#ff5555";
  ctx.font = "12px monospace";
  ctx.textBaseline = "top";
  ctx.fillText("RUNTIME ERROR (see console):", 8, 8);
  const words = message.split(/\s+/);
  let line = "";
  let y = 28;
  const maxChars = Math.floor((canvas.width - 16) / 7);
  for (const w of words) {
    if ((line + " " + w).length > maxChars) { ctx.fillText(line, 8, y); y += 14; line = w; }
    else line = line ? line + " " + w : w;
  }
  if (line) ctx.fillText(line, 8, y);
}

export function guard(canvas: HTMLCanvasElement, run: () => Promise<void>): void {
  const onErr = (msg: string) => fatalOverlay(canvas, msg);
  window.addEventListener("error", (e) => onErr(e.message));
  window.addEventListener("unhandledrejection", (e) => onErr(String(e.reason)));
  run().catch((err) => onErr(err?.stack ?? String(err)));
}
