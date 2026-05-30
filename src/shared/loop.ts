// Fixed-timestep ticker. The 1990 original ran its logic at the PAL frame
// rate (~50 Hz); we decouple logic ticks from render so animation feel stays
// constant regardless of display refresh. (House rule: fixed-timestep loop.)
export const TICK_HZ = 50;
const TICK_MS = 1000 / TICK_HZ;
const MAX_CATCHUP = 5; // never spiral if a tab was backgrounded

export interface Loop {
  stop(): void;
}

export function startLoop(tick: () => void, render: () => void): Loop {
  let acc = 0;
  let last = performance.now();
  let raf = 0;
  let running = true;

  const frame = (now: number) => {
    if (!running) return;
    acc += now - last;
    last = now;
    let steps = 0;
    while (acc >= TICK_MS && steps < MAX_CATCHUP) {
      tick();
      acc -= TICK_MS;
      steps++;
    }
    if (steps === MAX_CATCHUP) acc = 0;
    render();
    raf = requestAnimationFrame(frame);
  };
  raf = requestAnimationFrame(frame);

  return {
    stop() {
      running = false;
      cancelAnimationFrame(raf);
    },
  };
}
