import { resolve } from "node:path";
import { defineConfig } from "vite";

const here = import.meta.dirname;

// Multi-page: the menu, and the two standalone slices (intro, terminal).
export default defineConfig({
  base: "./",
  build: {
    target: "es2022",
    rollupOptions: {
      input: {
        menu: resolve(here, "index.html"),
        intro: resolve(here, "intro.html"),
        terminal: resolve(here, "terminal.html"),
        decks: resolve(here, "decks.html"),
        flight: resolve(here, "flight.html"),
        lore: resolve(here, "lore.html"),
        sounds: resolve(here, "sounds.html"),
      },
    },
  },
});
