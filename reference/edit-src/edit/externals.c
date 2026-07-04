#include <exec/execbase.h>
#include <exec/exec.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <libraries/dos.h>
#include <hardware/custom.h>
#include <hardware/blit.h>

#define MOVE(a, b) *copperlist++ = a & 0x1fe; *copperlist++ = b
#define WAIT(a, b) *copperlist++ = (a<<8)|b|1; *copperlist++ = 0xfffe
#define WAITB(a, b) *copperlist++ = (a<<8)|b|1; *copperlist++ = 0x7ffe
#define WAITE(a, b, c, d) *copperlist++ = (a<<8)|b|1; *copperlist++ = ((c<<8)|d)&0xfffe
#define SKIP(a, b) *copperlist++ = (a<<8)|b|1; *copperlist++ = 0xffff
#define SKIPE(a, b, c, d) *copperlist++ = (a<<8)|b|1; *copperlist++ = (c<<8)|d|1

#define MAXPLANES  5L
#define WIDTH      304L
#define HEIGHT     192L

#define WIDTHS     640L

#define IHEIGHT    22L

#define MAXDIGITS  5
