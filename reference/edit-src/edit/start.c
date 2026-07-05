#include <stdio.h>

extern struct ExecBase *SysBase;

UWORD colortable [] = {
    0x0,0xFB9,0xDB2,0xB90,0xD97,0xB75,0x223,0x99A,
    0x778,0x556,0x334,0xE26,0xA13,0x701,0x300,0xF00,
    0x778,0x667,0x445,0x223,0xBCC,0x788,0x677,0x566,
    0x455,0x344,0x233,0x122,0x11,0xCCD,0x674,0x112
/*	0x0,0xFB9,0xDB2,0xB90,0xD97,0xB75,0x224,0x99B,
	0x779,0x557,0x335,0xAA,0x77,0x44,0xDD,0xF00,
	0x778,0x556,0x445,0x334,0x223,0xA00,0x800,0x600,
	0x655,0x544,0x433,0x211,0x322,0xBBD,0x974,0x691*/
};

/*	0x000,0xFC9,0xFF2,0xCB0,
	0xEA8,0xC86,0x03B,0xFFF,
	0xAAC,0x668,0x335,0x000,
	0x000,0x000,0x000,0x000,
	0x079,0x057,0x046,0x035,
	0x024,0xA00,0x800,0x600,
	0xA77,0x855,0x533,0x311,
	0x422,0x451,0x571,0x691*/


UWORD colortable2 [] = {
    0x0,0xFC9,0xFF2,0xCB0,0xEA8,0xC86,0x3B,0xFFF,
    0xAAC,0x668,0x335,0xFF,0xCC,0xA9,0x77,0xF00,
    0x79,0x57,0x46,0x35,0x24,0xA00,0x800,0x600,
    0xA77,0x855,0x533,0x311,0x422,0x451,0x571,0x691
};

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct FileHandler *fh, *Open ();
UWORD *AllocMem ();
long AvailMem ();
char *OpenLibrary ();

UWORD *bpptr[MAXPLANES], *delayptr, *wptr, *sprptr, xpos;
UWORD matrix[12][128];
UWORD *ground[512];

UWORD *bobmem[8], *objmem[8];

long BitPlaneA [MAXPLANES], BitPlaneB [MAXPLANES], BitPlane2[MAXPLANES];
UWORD *spritechannel[2][8];
UWORD vspriteon[16], vspritex[16], vspritey[16], vspritedma[16], *vspritei[16];
UWORD sprites;
int square, square2, *squaremem;

UWORD *copperlist, *clist, *colptr;

UWORD *bob;

long ftell ();
FILE *fopen ();
FILE *fptr;
int gno, sc;

extern long score [2];

init_sprite ()
{
register int i, j;

	for( i = 0; i < 8; i ++ ) for( j = 0; j < 2; j ++ ) 
		spritechannel[j][i] = AllocMem( 1500L, MEMF_CHIP | MEMF_CLEAR);
}

load_sprite ()
{
register int i;

	fh = Open ("gfx:Square", MODE_OLDFILE);
	squaremem = (int *)AllocMem( 130L, MEMF_CLEAR );
	Read (fh, squaremem , 130L);
	Close (fh);


}

load_bobs ()
{

	bob = AllocMem( 1730L, MEMF_CHIP );

	fh = Open ("gfx:bob", MODE_OLDFILE);
	Read (fh, bob, 1730L);
	Close (fh);

}

prepare_bobmem ()
{
register int i;

	for( i = 0; i < 8; i ++ ) bobmem[i] = AllocMem( 3000L, MEMF_CHIP );

}

load_obj ()
{
register int i, j;
register UWORD *ptr;

	fh = Open ("gfx:objects", MODE_OLDFILE);
	for( i = 0; i < 7; i ++ ) {
		objmem[i] = (UWORD *)AllocMem( 194L, MEMF_CLEAR );
		Read (fh, objmem [i] , 194L);
		ptr = objmem[i] + 81;
		for( j = 0; j < 16; j ++ ) *ptr++ ^= 0xFFFF;
	}
	Close (fh);

}

load_matrix ()
{
register int x, y;

	if( (fh = Open ("gfx:matrix", MODE_OLDFILE)) > 0 ) {
		Read (fh, matrix, 3072L);
		Close (fh);
	}

}

save_matrix ()
{
register int i;

	if( (fh = Open ("gfx:matrix", MODE_NEWFILE)) > 0 ) {
		Write (fh, matrix, 3072L);
		Close (fh);
	}
	fh = Open ("gfx:elements", MODE_NEWFILE);
	Write (fh, &gno, 2L);
	for (i = 0; i < gno; i++) {
		Write (fh, ground [i], 160L);
	}
	Close (fh);
}

char name [] = "GFX:elem/elem00";

load_graphics ( nr )
register int nr;
{
long length;
register int i, ii;

	gno = 0;
	for( i = 0; i < nr; i ++ ) {
		fptr = fopen (name, "r");
		fseek (fptr, 0L, 2);
		length = ftell (fptr);
		fclose (fptr);
		fh = Open (name, MODE_OLDFILE);
		for (ii = 0; ii < length/160L; ii++) {
			ground [gno] = AllocMem (160L, MEMF_CHIP);
			Read (fh, ground [gno++], 160L);
		}
		Close (fh);
		puts (name);
		if (name [14] == '9') {
			name [14] = '0';
			++name [13];
		} else ++name [14];
	}
}

open_view ()
{
long ptr1, ptr2, ptr3;
register int i, r, s;
register UWORD *ptr;
register long c;

	GfxBase = (struct GfxBase *) OpenLibrary ("graphics.library", 0L);
	IntuitionBase = (struct IntuitionBase *) OpenLibrary ("intuition.library", 0L);

	clist = copperlist = AllocMem (500L, MEMF_CHIP);

	ptr1 = (long) AllocMem (MAXPLANES*WIDTHS*HEIGHT/8, MEMF_CHIP | MEMF_CLEAR);
	ptr3 = (long) AllocMem (MAXPLANES*WIDTHS*HEIGHT/8, MEMF_CHIP | MEMF_CLEAR);

	ptr2 = (long) AllocMem (MAXPLANES*WIDTH*IHEIGHT/8, MEMF_CHIP/* | MEMF_CLEAR*/);
	ptr = (UWORD *)ptr2;
	for( c = 0; c < MAXPLANES*WIDTH*IHEIGHT/16; c ++ ) *ptr ++ = 0xFFFF;

	for (i = 0; i < MAXPLANES; i++) BitPlaneA[i]  = ptr1 + i*(WIDTHS*HEIGHT/8);
	for (i = 0; i < MAXPLANES; i++) BitPlaneB[i]  = ptr3 + i*(WIDTHS*HEIGHT/8);

	for (i = 0; i < MAXPLANES; i++) BitPlane2[i] = ptr2 + i*(WIDTH*IHEIGHT/8);

	MOVE( 0x100, 0x200 ); /* BPLCONx */
	MOVE( 0x102, 0x0000 );
	MOVE( 0x104, 0x0024 );
	MOVE( 0x092, 0x0040 ); /* DDFSTART */
	MOVE( 0x094, 0x00D0 ); /* DDFSTOP  */
	MOVE( 0x08E, 0x0591 ); /* DIWSTART */
	MOVE( 0x090, 0xFFC1 ); /* DIWSTOP  */
	MOVE( 0x108, 0x0000 ); /* BPL1MOD  */
	MOVE( 0x10A, 0x0000 ); /* BPL2MOD  */

	sprptr = copperlist + 1;
	MOVE( 0x120, (long)spritechannel[0][0] >> 16 );   /* SPRITE DATA 0 */
	MOVE( 0x122, (long)spritechannel[0][0] & 0xffff); /* SPRITE DATA 0 */
	MOVE( 0x124, (long)spritechannel[0][1] >> 16 );   /* SPRITE DATA 1 */
	MOVE( 0x126, (long)spritechannel[0][1] & 0xffff); /* SPRITE DATA 1 */
	MOVE( 0x128, (long)spritechannel[0][2] >> 16 );   /* SPRITE DATA 2 */
	MOVE( 0x12A, (long)spritechannel[0][2] & 0xffff); /* SPRITE DATA 2 */
	MOVE( 0x12C, (long)spritechannel[0][3] >> 16 );   /* SPRITE DATA 3 */
	MOVE( 0x12E, (long)spritechannel[0][3] & 0xffff); /* SPRITE DATA 3 */
	MOVE( 0x130, (long)spritechannel[0][4] >> 16 );   /* SPRITE DATA 4 */
	MOVE( 0x132, (long)spritechannel[0][4] & 0xffff); /* SPRITE DATA 4 */
	MOVE( 0x134, (long)spritechannel[0][5] >> 16 );   /* SPRITE DATA 5 */
	MOVE( 0x136, (long)spritechannel[0][5] & 0xffff); /* SPRITE DATA 5 */
	MOVE( 0x138, (long)spritechannel[0][6] >> 16 );   /* SPRITE DATA 6 */
	MOVE( 0x13A, (long)spritechannel[0][6] & 0xffff); /* SPRITE DATA 6 */
	MOVE( 0x13C, (long)spritechannel[0][7] >> 16 );   /* SPRITE DATA 7 */
	MOVE( 0x13E, (long)spritechannel[0][7] & 0xffff); /* SPRITE DATA 7 */

for( i = 0; i < (1 << MAXPLANES); i ++ ) {MOVE( 0x180+2*i, colortable2[i] );}
	for( i = 0; i < MAXPLANES; i ++ ) {
		MOVE( 0xe0 + i*4, BitPlane2[i] >> 16);
		MOVE( 0xe2 + i*4, BitPlane2[i] & 0xffff);
	}
	WAIT( 0x027, 0x000 );
	MOVE( 0x100, (MAXPLANES << 12) | 0x200 ); /* BPLCON0 */

	WAIT( 0x03D, 0x000 );
	MOVE( 0x100, 0x0200 ); /* BPLCON0  */
	delayptr = copperlist + 1;
	MOVE( 0x102, 0x0000 ); /* BPLCON1  */
	MOVE( 0x092, 0x0038 ); /* DDFSTART */
	MOVE( 0x108, 0x0028 ); /* BPL1MOD  */
	MOVE( 0x10A, 0x0028 ); /* BPL2MOD  */
	colptr = copperlist;
	colptr += 2*11+1;
for( i = 0; i < (1 << MAXPLANES); i ++ ) {MOVE( 0x180+2*i, colortable[i] );}

	for( i = 0; i < MAXPLANES; i ++ ) {
		bpptr[i] = copperlist + 1;
		MOVE( 0xe0 + i*4, (BitPlaneA[i]-2) >> 16);
		MOVE( 0xe2 + i*4, (BitPlaneA[i]-2) & 0xffff);
	}

	WAIT( 0x03F, 0x000 );
	MOVE( 0x100, (MAXPLANES << 12) | 0x200 ); /* BPLCON0 */

	wptr = copperlist;
	WAITB( 0x0FF, 0x000 );
	MOVE( 0x09C, 0x8040 );
	WAIT( 0x0FF, 0x000 );
	MOVE( 0x100, 0x0200 ); /* BPLCON0 */
	WAIT( 0xFF, 0xFE   );

/*	for( i = 0; i < 150; i += 2 ) printf( "%x %x\n", *(clist+i)*(clist+i+1) );
*/	custom.cop1lc  = (long)clist;

}
