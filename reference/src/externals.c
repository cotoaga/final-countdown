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

#define WIDTH      304L
#define WIDTHS     640L
#define HEIGHT     192L
#define IHEIGHT    22L

#define TITLEPLANES 2L
#define TITLEWIDTH 640L
#define TITLEHEIGHT 45L

#define MAXPLANES  5L
#define MAXDIGITS  5
#define MAXDEVS    32
#define MAXCHANNEL 8
#define MAXFOES    32
#define MAXLEVELS  8
#define MAXCOMPS   16

#define LIFTLIFE   65

#define MFLASH     21
#define MGFLASH    63
#define MLIFT      33
#define MAGRAV     67
#define MSWITCH    6
#define MHIDE      12
#define MGROUND    0x8000
#define MENERGY    0x4000
#define MMINE      0x0800
#define MTRAP      0x1000
#define MFUEL      0x1800
#define MDISK1     0x2000
#define MDISK2     0x2800
#define MDISK3     0x3000
#define MDISK4     0x3800
#define CHECKTHING 0x3800
#define MCOMPUTER  0
#define MFOE       1
#define FRGREL     51
#define LAGREL     56

#define SMGROUND   0
#define SMLIFTUP   1
#define SMLIFTDN   2
#define SMSTARTR   3
#define SMSTARTL   4
#define SMSTOPR    5
#define SMSTOPL    6
#define SMFLY      7
#define SMTURNS    8
#define SMTURNE    9
#define SMSAIRR   10
#define SMSAIRL   11
#define SMEAIRR   12
#define SMEAIRL   13
#define SMHIDEB   14
#define SMHIDEE   15
#define SMTAKED   16
#define SMTAKEU   17
#define SMDIE     18
#define SMUP      19
#define SMDOWN    20

