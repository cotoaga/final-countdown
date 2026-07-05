#define MAXCHANNEL 8
#define MAXGE      512

extern long BitPlaneA[MAXPLANES], BitPlaneB[MAXPLANES], BitPlane2[MAXPLANES];
extern UWORD *bob;
extern UWORD *ground[MAXGE];
extern UWORD matrix[12][128];
extern UWORD *bpptr[MAXPLANES], *delayptr, *wptr, *sprptr, xpos;
int xdiff, lxdiff, xscreen, xtarget;
int joyx, joyy, button, njoyx, njoyy, akground;
char *buttonptr = 0xbfe001;
char *addr = 0xBFEC01;

extern long score [2];

struct Interrupt BlitterInt;

void blitter_int ();

UWORD bobon[8], bobx[8], boby[8], *bobimage[8];

UWORD boblistx[4], boblisty[4], *boblisti[5];
UWORD clrlistx[4], clrlisty[4], clrlistdx[5], clrlistdy[4];
int worklist[9];

WORD akbob, akplane;

int bob1, bob2, bob3, bob4;
int colcount = 0, colno = 0;

extern UWORD *spritechannel[2][8];
extern UWORD vspriteon[16], vspritex[16], vspritey[16], vspritedma[16], *vspritei[16];
extern UWORD sprites;
int channely[4], slist[16];
UWORD *spritechannelA[8];
int dbuf, sc;

extern UWORD *colptr;

extern int square, square2, *squaremem;

extern void copy_ground ();

int setmodex = 1, setmodey = 1, setmode;
int flag;

int colcounter = 0;
int coldir = 1;

main ()
{
register int i, x, y;
int xx, yy;
char buf [40], *gets ();

	dbuf = 0;

	printf ("Enter Number Of Elements: ");
	i = atoi (gets (buf));
	load_graphics (i);
	init_sprite ();
	open_view ();
	load_obj ();
	load_matrix ();
	load_sprite ();
	init_scr ();

	Forbid ();

	for( x = 0; x < 20; x ++ )
		for( y = 0; y < 12; y ++ ) {
			copy_ground_edit( x, y, x );
			if( x < 19 ) copy_ground_edit( x, y, x + 21);
	}


	square = add_sprite();
	vspritex[square] = 0;
	vspritey[square] = 0;
	vspritei[square] = squaremem;
	square2 = add_sprite();
	vspritex[square2] = 0;
	vspritey[square2] = 0;
	vspritei[square2] = squaremem;



	akbob = 0;
	akplane = 0;

	custom.dmacon = DMAF_SETCLR | DMAF_SPRITE;
	xdiff = 0;
	copy_ground2( ground[0] );
	score[0] = 0;
	for(;;) {
		WaitTOF ();
		flag = 0;

		switch (rand () & 15) {
			case 15:	*colptr = 0x0ff;
						break;
			case 14:	*colptr = 0x000;
						break;
			case 13:	*(colptr+2) = 0x0cc;
						break;
			case 12:	*(colptr+2) = 0x000;
						break;
			case 11:	*(colptr+4) = 0x099;
						break;
			case 10:	*(colptr+4) = 0x000;
						break;
			case 9:	*(colptr+6) = 0x066;
						break;
			case 8:	*(colptr+6) = 0x000;
		}
		if (coldir > 0) ++colcounter;
		else --colcounter;
		if (colcounter > 2*15) coldir = -1;
		if (colcounter < 1) coldir = 1;
		*(colptr+8) = ((colcounter >> 1) << 8);

		ScrollBG ();

		DrawBobs ();

		joystick ();
		if( joyy > 0 && njoyy < 8 ) if( ++njoyy < 1 ) njoyy = 1;
		if( joyx > 0 && njoyx < 8 ) if( ++njoyx < 1 ) njoyx = 1;
		if( joyy < 0 && njoyy > -8 ) if( --njoyy > -1 ) njoyy = -1;
		if( joyx < 0 && njoyx > -8 ) if( --njoyx > -1 ) njoyx = -1;
		if( njoyy < -7 && vspritey[square] > 0 ) {
			vspritey[square] -= 16, njoyy = 0, flag = 1;
		}
		if( njoyy >  7 && vspritey[square] < 192 - setmodey*16) {
			vspritey[square] += 16, njoyy = 0, flag = 1;
		}
		if( njoyx < -7 && vspritex[square] > 0 ) {
			vspritex[square] -= 16, njoyx = 0, flag = 1;
		}
		if( njoyx >  7 && vspritex[square] < 304 - setmodex*16) {
			vspritex[square] += 16, njoyx = 0, flag = 1;
		}
		vspritex[square2] = vspritex[square] + 16*(setmodex-1);
		vspritey[square2] = vspritey[square] + 16*(setmodey-1);
		if( button ) {
			y = vspritey[square] / 16;
			x = (vspritex[square] + xpos) / 16;
			for (yy = 0; yy < setmodey; yy++)
				for (xx = 0; xx < setmodex; xx++) {
					matrix[y+yy][x+xx] = akground+setmodex*yy+xx | setmode | ((akground == 21 || akground == 63 || akground == 80)? 0x4000: 0);
					i = (vspritex[square] + xscreen)/16+xx;
					copy_ground_edit( x+xx, y+yy, i );
					if( i < 21 ) copy_ground_edit( x+xx, y+yy, i + 21);
					if( i > 20 ) copy_ground_edit( x+xx, y+yy, i - 21);
				}
		}
		if( xdiff == 0 ) i = key();
		else i = -1;
		if( i == 0 ) {
			Permit ();
			save_matrix ();
			Delay( 400L );
			Forbid ();
		}
		if( i == 2 ) {
			xtarget = xpos + 144;
			xdiff = 1;
			if( xtarget > 1728 ) xtarget = 1728;
		}
		if( i == 1 ) {
			xtarget = xpos - 144;
			xdiff = -1;
			if( xtarget < 0 ) xtarget = 0;
		}
		if( i == 3 ) {
			if( akground >= setmodex*setmodey ) akground -= setmodey*setmodex;
		}
		if( i == 4 ) {
			if( akground < MAXGE-setmodex*setmodey ) akground += setmodey*setmodex;
		}
		if( i == 3 || i == 4 ) copy_ground2( ground[akground] );
		y = vspritey[square] / 16;
		x = vspritex[square] / 16;
		if( i == 5) setmodex += 1;
		if (x+setmodex > 19 || setmodex > 4) setmodex = 1;
		if( i == 6) setmodey += 1;
		if (y+setmodey > 12 || setmodey > 3) setmodey = 1;

		if( i == 7 ) setmode ^= 0x8000;

		if( i == 8 ) {
			y = vspritey[square] / 16;
			x = (vspritex[square] + xpos) / 16;
			inc_obj( x, y );
		}

		if( xpos == xtarget ) {
			xdiff = 0;
			flag = 1;
		}

		if( flag ) {
			score[0] = (long)(((xpos + vspritex[square]) / 16) + (vspritey[square] / 16)*1000L);
			new_score ();
		}
		refresh_score ();
		SpriteMachine ();
	}

}


inc_obj ( x, y )
register int x, y;
{
register int nr;

	nr = (matrix[y][x] & 0x3800) >> 11;
	nr = (nr+1) & 7;
	matrix[y][x] = (matrix[y][x] & 0xC7FF) | (nr << 11);
	if( x-(xpos>>4) > 0 && x-(xpos>>4) < 20 ) {
		nr = x-(xpos>>4)+((xpos%336)>>4);
		copy_ground_edit(x, y, nr);
		copy_obj( x, y, nr);
		if( nr+21 < 40 ) {
			copy_ground_edit(x, y, nr+21);
			copy_obj( x, y, nr+21);
		}
		if( nr-21 >= 0 ) {
			copy_ground_edit(x, y, nr-21);
			copy_obj( x, y, nr-21);
		}
	}

}

ScrollBG ()
{
register int y, x, i;
register long j;

	x = xpos % 336;

	if( xdiff > 0 ) {
		y = (xpos & 15) - 4;
		if( y >= 0 ) {
			if(x < 320) {
				copy_ground_edit((xpos+320)>>4, y, (x + 320)>>4);
				if( matrix[y][(xpos+320)>>4] & 0xCFFF ) copy_obj((xpos+320)>>4, y, (x + 320)>>4);
			}
			if(x > 15) {
				copy_ground_edit((xpos+320)>>4, y, (x - 16)>>4);
				if( matrix[y][(xpos+320)>>4] & 0xCFFF ) copy_obj((xpos+320)>>4, y, (x - 16)>>4);
			}
		}
	} else if( xdiff < 0 ) {
		y = xpos & 15;
		if( y <= 11 ) {
			if(x < 320 && xpos > 15) {
				copy_ground_edit((xpos-16)>>4, y, (x + 320)>>4);
				if( y <= 10 && (matrix[y+1][(xpos-16)>>4] & 0xCFFF) ) copy_obj((xpos-16)>>4, y+1, (x + 320)>>4);
			}
			if(x > 15) {
				copy_ground_edit((xpos-16)>>4, y, (x - 16)>>4);
				if( y <= 10 && (matrix[y+1][(xpos-16)>>4] & 0xCFFF) ) copy_obj((xpos-16)>>4, y+1, (x - 16)>>4);
			}
		}
	}

	xpos += xdiff;
	xscreen = x = xpos % 336;

	if( x == 0 ) xscreen = x = 336;
	for( i = 0; i < MAXPLANES; i ++ ) {
		j = BitPlaneA[i] + ((x-1) / 16)*2;
		*bpptr[i]     = j >> 16;
		*(bpptr[i]+2) = j & 0xFFFF;
	}

	i = ((16-(x&15)) & 15);
	*delayptr = i + (i<<4);
}

add_bob ()
{
register int i;

	i = 0;
	while( bobon[i] && i < 4 ) ++ i;
	if( i == 4 ) i = -1;
	else bobon[i] = 1;

return i;
}

rem_bob ( nr )
register UWORD nr;
{
	bobon[nr] = 0;
}

DrawBobs ()
{
register int i, j, k, y;
int count;

	count = 0;
	for( i = 0; i < 4; i ++ )
		if (bobon [i]) count++;

	for (i = 0; i < count; i++) {
		for( y = 666, j = 0, k = 0; j < 4; j ++ )
			if (bobon [j] == 1 && boby [j] <= y) {
				y = boby [j];
				k = j;
			}
		bobon [k] = 2;
		boblisty [i] = boby [k];
		boblistx [i] = bobx [k];
		boblisti [i] = bobimage [k];
	}
	boblisti [count] = 0;

	for( i = 0; i < 4; i ++ )
		if (bobon [i]) bobon [i] = 1;

	if( count ) *wptr = ((boblisty[0] + (*boblisti[0] & 0xFF) + 0x3F) << 8) | 1;

}


add_sprite ()
{
register int i;

	for( i = 0; vspriteon[i]; i ++ );
	vspriteon[i] = 1;
	slist[sprites] = i;
	sprites ++;

return i;
}

remove_sprite ( nr )
register int nr;
{
register int i;

	if( nr > -1 && vspriteon[nr] ) {
		vspriteon[nr] = 0;
		for( i = 0; slist[i] != nr; i ++ );
		for( ; i < sprites; i ++ ) slist[i] = slist[i+1];
		sprites --;
	}
}

joystick( )
{
	register int i = 0, cond;
	static int lbutton = 0, b, lmove = 0;

	joyy = (-(((custom.joy1dat & 256) != 0) ^ ((custom.joy1dat & 512) != 0)));
	joyy = (joyy < 0)? -1: (((custom.joy1dat & 2) != 0) ^ ((custom.joy1dat & 1) != 0));
	joyx = (-((custom.joy1dat & 512) != 0));
	joyx = (joyx < 0)? -1: ((custom.joy1dat & 2) != 0);
	b    = ((*buttonptr & 0x80) == 0);
	if (b && !lbutton) button = 1;
	else button = 0;
	lbutton = b;
}


char keycode[] = {
0xEB, 0xFD, 0xFB, 0xF9, 0xF7, 0xF5, 0xF3, 0xF1, 0xEF, 0xED,
0xBF, 0x95, 0x99, 0xBB, 0xDB, 0xB9, 0xB7, 0xB5, 0xD1, 0xB3,
0xB1, 0xAF, 0x91, 0x93, 0xCF, 0xCD, 0xDF, 0xD9, 0xBD, 0xD7,
0xD3, 0x97, 0xDD, 0x9B, 0x9D, 0xD5, 0x8D, 0x5F, 0x5D, 0x5B,
0x59, 0x57, 0x7F, 0x7D, 0x77
};


key ()
{
register char c;
register int i;

	i = 45;
	c = *addr;
	if( c & 1 ) while( *addr & 1 );
	for( i = 0; i < 45; i ++ ) if( keycode[i] == c ) break;

	if( i == 45 ) i = -1;

return i;
}

copy_ground2( image )
register UWORD *image;
{
register UWORD *ptr;
register int add, j, i;

	for (i = 0; i < MAXPLANES; i++) {
		ptr = (UWORD *)BitPlane2[i];
		for( j = 0; j < 16; j ++,ptr += WIDTH/16 ) *ptr = *image++;
	}
}

copy_ground_edit( x, y, xp )
int x, y, xp;
{
register UWORD *image, *ptrA, *ptrB;
register int add, j, i;

	image = ground[ (i = matrix[y][x]) & 0x01FF];
	add = (WIDTHS*2) * y + (xp << 1); 

	if( i & 0x8000) {
		for (i = 0; i < MAXPLANES; i++) {
			ptrA = (UWORD *)(BitPlaneA[i] + add);
			ptrB = (UWORD *)(BitPlaneB[i] + add);
			for( j = 0; j < 16; j ++,ptrA += WIDTHS/16, ptrB += WIDTHS/16 ) *ptrB = *ptrA = *image++ & (0x5555 << (j&1));
		}
	} else {
		for (i = 0; i < MAXPLANES; i++) {
			ptrA = (UWORD *)(BitPlaneA[i] + add);
			ptrB = (UWORD *)(BitPlaneB[i] + add);
			for( j = 0; j < 16; j ++,ptrA += WIDTHS/16, ptrB += WIDTHS/16 ) *ptrB = *ptrA = *image++;
		}
	}
}
