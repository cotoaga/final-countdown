// Landing menu is static HTML; this entry exists so Vite treats index.html as
// an app page and to keep keyboard affordances consistent.
const links = Array.from(document.querySelectorAll<HTMLAnchorElement>(".menu-links a"));
window.addEventListener("keydown", (e) => {
  const n = Number(e.key);
  if (n >= 1 && n <= links.length) links[n - 1]?.click();
});
