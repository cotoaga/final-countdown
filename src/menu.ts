// Landing menu is static HTML; this entry exists so Vite treats index.html as
// an app page and to keep keyboard affordances consistent.
const links = Array.from(document.querySelectorAll<HTMLAnchorElement>(".menu-links a"));
window.addEventListener("keydown", (e) => {
  if (e.key === "1") links[0]?.click();
  if (e.key === "2") links[1]?.click();
});
