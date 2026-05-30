extern LONG BitPlaneA [MAXPLANES];

extern WORD button;
extern WORD countdown, sintable [360], esc, green;
extern WORD flying, collision, specmv, myxd, level, life, master;
extern WORD exno, mines, oldmines, path, disk, expfn, fuel;
extern LONG mxp, gtime, score;
WORD leveladd, setobjx, setobjy, fueladd;
WORD dark, message, statr, statg, statb;
WORD habeilev [MAXLEVELS];

struct IntuitionBase *IntuitionBase;
extern BYTE *OpenLibrary ();
LONG AvailMem();
UWORD lomem;

extern UWORD xpos, infected [];
extern UWORD *bpptr [MAXPLANES], *delayptr, *wptr, *sprptr;
extern UWORD *colptr, *colptr2;
extern UWORD matrix [15][128];
extern UWORD vspriteon [16], sprites;

extern VOID copy_ground ();

extern WORD air_level[MAXLEVELS][5];

struct Interrupt BlitterInt, SoundInterrupt, CompInterrupt;
struct Interrupt *BlitterIntOld, *SetIntVector ();
void blitter_int (), computer_int (), effect_int (), SoundInt ();

VOID EffectInt0 (), EffectInt1 (), EffectInt2 (), EffectInt3 ();
struct Interrupt EffectInterrupt [4];

LONG itime, ttime, ccount;
WORD xmul, ymul, clength, cnum, title_on;
WORD sinnx, sinny, ftime;
WORD xdiff, xscreen;
WORD dbuf, akbob, akplane;
WORD channely [4], slist [16];
WORD mass, dka, dkb, dkc, chan [4], eff [4], justdied;
WORD trans, seldes, gorg, fatal, xray, intal, tarpen, viccol, hygkoo, ekiman, novbom, steexc, tacdes, semcom;
UWORD *spritechannelA [8];
UWORD *boblisti, *bobimage;
UWORD bobon, bobx, boby, boblistx, boblisty;
UWORD clrlistx, clrlisty, clrlistdx, clrlistdy;

WORD cyccol [8][4][4] = {
{	{ 0x066, 0x099, 0x0cc, 0x0ff },
	{ 0x099, 0x0cc, 0x0ff, 0x066 },
	{ 0x0cc, 0x0ff, 0x066, 0x099 },
	{ 0x0ff, 0x066, 0x099, 0x0cc }},
{	{ 0x222, 0x666, 0xbbb, 0xfff },
	{ 0x666, 0xbbb, 0xfff, 0x222 },
	{ 0xbbb, 0xfff, 0x222, 0x666 },
	{ 0xfff, 0x222, 0x666, 0xbbb }},
{	{ 0x222, 0x666, 0xbbb, 0xfff },
	{ 0x666, 0xbbb, 0xfff, 0x222 },
	{ 0xbbb, 0xfff, 0x222, 0x666 },
	{ 0xfff, 0x222, 0x666, 0xbbb }},
{	{ 0x066, 0x099, 0x0cc, 0x0ff },
	{ 0x099, 0x0cc, 0x0ff, 0x066 },
	{ 0x0cc, 0x0ff, 0x066, 0x099 },
	{ 0x0ff, 0x066, 0x099, 0x0cc }},
{	{ 0xd26, 0xa13, 0x701, 0x501 },
	{ 0xa13, 0x701, 0x501, 0xd26 },
	{ 0x701, 0x501, 0xd26, 0xa13 },
	{ 0x501, 0xd26, 0xa13, 0x701 }},
{	{ 0xd26, 0xa13, 0x701, 0x501 },
	{ 0xa13, 0x701, 0x501, 0xd26 },
	{ 0x701, 0x501, 0xd26, 0xa13 },
	{ 0x501, 0xd26, 0xa13, 0x701 }},
{	{ 0xd26, 0xa13, 0x701, 0x501 },
	{ 0xa13, 0x701, 0x501, 0xd26 },
	{ 0x701, 0x501, 0xd26, 0xa13 },
	{ 0x501, 0xd26, 0xa13, 0x701 }},
{	{ 0xf00, 0xc00, 0x700, 0x300 },
	{ 0xc00, 0x700, 0x300, 0xf00 },
	{ 0x700, 0x300, 0xf00, 0xc00 },
	{ 0x300, 0xf00, 0xc00, 0x700 }}
};

WORD fuelval [MAXLEVELS][2] = {
	{ 150, 100 },
	{ 200, 100 },
	{ 150, 100 },
	{ 100, 100 },
	{ 200, 100 },
	{ 100, 100 },
	{ 100, 100 },
	{ 100, 100 }
};

WORD air_bad = 0x80;

WORD titable [] = {
	0xFE1,
	0xF00,
	0x0E0,
	0xFE1,
	0xF00,
}, tii;

init_int1 ()
{
	SoundInterrupt.is_Node.ln_Type = NT_INTERRUPT;
	SoundInterrupt.is_Node.ln_Pri  = 0;
	SoundInterrupt.is_Node.ln_Name = "SoundInt";
	SoundInterrupt.is_Data = 0;
	SoundInterrupt.is_Code = SoundInt;

	AddIntServer( INTB_VERTB, &SoundInterrupt );
}

rem_int1 ()
{

	RemIntServer( INTB_VERTB, &SoundInterrupt );
}

void SoundInt ()
{
#asm
	movem.l d5-d7/a4,-(sp)
	jsr _geta4#

	jsr mt_music
	public mt_music

	movem.l (sp)+,d5-d7/a4
	moveq.l	#1,D0
#endasm
}

main ()
{
register int i, x;

	IntuitionBase = (struct IntuitionBase *) OpenLibrary ("intuition.library", 0L);

	if( AvailMem( NULL ) < 500000L && AvailMem( MEMF_FAST ) < 10000 ) lomem = 1;
	else lomem = 0;

	init_int4 ();
	init_sprite ();
	load_music ();

	CloseWorkBench ();

	init_int1 ();
	title_pic ();
	free_credits ();

	open_view ();
	load_stuff ();

	init_int3 ();

	xmul = 5;
	ymul = 7;
	clength = 20;
	cnum = 1;
	Forbid ();
	init_int2 ();
	akbob = 0;
	akplane = 0;

	on_blit ();

	load_syssounds ();
	load_sounds ();

	disk_change ();
	end_titlepic ();
	custom.dmacon = DMAF_RASTER | DMAF_SPRITE;

	load_syscomp ();
	for(;;) {
		for (i = 0; i < MAXLEVELS; i++) {
			habeilev [i] = 0;
			infected [i] = 0;
		}
		message = -1;
		justdied = countdown = mass = ttime = gorg = xray = disk = path = dark = itime = tarpen = viccol = hygkoo = 0;
		green = dka = dkb = dkc = trans = seldes = fatal = intal = exno = ekiman = novbom = steexc = tacdes = semcom = 0;
		air_level [7][0] = air_level [7][1] = air_level [7][2] = 0;
		air_level [7][3] = 7;
		air_level [7][4] = 3;
		air_bad = 0x80;

		read_text ( "DISK01:TXT/MsgA" );
		title ();

		marmor_on ();

		gtime = fuel = 0;
		fueladd = 200;
		level = mines = 0;
		ftime = oldmines = -1;

		init_scr ();

		for (life = 1, leveladd = 1; life > 0;) {
			if ((green >= 30 && !leveladd) || (xray >= 100 && !leveladd) || novbom >= 30 || seldes >= 200) break;
			green = (air_bad >> level) & 1;
			if (leveladd) {
				justdied = 0;
				off_blit ();
				load_graphics (level);
				load_matrix (level);
				load_struct (level);
				init_foes (level);
				restore_devices (level);
				on_blit ();
				collision = leveladd = 0;
			} else {
				off_blit ();
				load_matrix (level);
				init_foes (level);
				restore_devices (level);
				on_blit ();
				fuel = 0;
				fueladd = fuelval [level][exno];
				justdied = 0;
			}
			do_sound (4);        /* background */
			init_move (level);
			score = 0;
			new_score ();
			new_mines ();

			for(;;) {
				move_own ();
				move_foes ();
				test_key ();
				if (leveladd && expfn == -1) {
					countdown = chan [0] = eff [0] = 0;
					store_devices (level);
					color_dark (dark);
					level += leveladd;
					bobon = 0;
					move_own ();
					while (akbob);
					DrawBobs ();
					akbob = -1;
					kill_all_foes ();
					SpriteMachine ();
					break;
				}
				if (trans == 1 && level == 1) {
					x = mxp >> 9;
					if (x > 50 && x < 70) {
						countdown = 0;
						chan [0] = eff [0] = 0;
						flash ();
						color_dark (dark);
						fuel_out ();
						bobon = 0;
						move_own ();
						while (akbob);
						DrawBobs ();
						akbob = -1;
						kill_all_foes ();
						SpriteMachine ();
						trans = 2;
						life = 0;
						break;
					}
				}
				if (master || specmv == SMLIFTUP || specmv == SMLIFTDN) collision = 0;
				if (collision > 1) {
					++collision;
					statr = 15<<4;
					statg = statb = 0;
				}
				if (collision >= 200 && expfn == -1) {
					justdied = 1;
					chan [0] = eff [0] = 0;
					store_devices (level);
					fuel_out ();
					color_dark (dark);
					bobon = 0;
					move_own ();
					while (akbob);
					DrawBobs ();
					akbob = -1;
					countdown = collision = esc = 0;
					kill_all_foes ();
					SpriteMachine ();
					leveladd = 0;
					if (itime >= 9900 && itime < 9990) { gtime += (9990L-itime)<<6; itime = 9990; }
					else if (itime >= 9500 && itime < 9900) { gtime += (9900L-itime)<<6; itime = 9900; }
					else if (itime >= 9000 && itime < 9500) { gtime += (9500L-itime)<<6; itime = 9500; }
					else if (itime >= 8000 && itime < 9000) { gtime += (9000L-itime)<<6; itime = 9000; }
					else if (itime >= 5000 && itime < 8000) { gtime += (8000L-itime)<<6; itime = 8000; }
					else if (itime >= 0 && itime < 5000) { gtime += (5000L-itime)<<6; itime = 5000; }
					break;
				}
				cycle_col ();
				while (akbob);

				DrawBobs ();
				ScrollBG ();
				SpriteMachine ();
				akbob = -1;
			}
		}
		marmor_off ();
		custom.dmacon = DMAF_AUDIO | DMAF_RASTER;

		bobon = 0;
		while (akbob);
		DrawBobs ();
		akbob = -1;
		while (akbob);
		DrawBobs ();
		akbob = -1;

		load_syscomp ();
		finished();
		clr_comp ();
		no_plot ();
	}
}

on_blit ()
{
	OwnBlitter ();
	SetIntVector (INTB_BLIT, &BlitterInt);
	custom.dmacon = DMAF_BLITHOG;
	custom.intena = INTF_SETCLR | INTF_BLIT;
}

off_blit ()
{
register long l;

	custom.intena = INTF_BLIT;
	for( l = 0; l < 60000; l ++ );
	SetIntVector (INTB_BLIT, BlitterIntOld);
	DisownBlitter ();
}

void EffectInt0 ()
{
#asm

	move.l	a4,-(sp)
	jsr	_geta4#

	tst.w	_eff
	bne	e0c0
	clr.w	_chan
	move.w	#$80,$9A(A0)
	bra	e0c1

e0c0:
	cmp.w	#10,_chan
	bgt	e0c1
	clr.w	_eff
	move.w	#1,$A4(A0)

e0c1:
	move.w	#$80,$9C(A0)
	move.l	(sp)+,a4

#endasm
}

void EffectInt1 ()
{
#asm

	move.l	a4,-(sp)
	jsr	_geta4#

	tst.w	_eff+2
	bne	e1c0
	clr.w	_chan+2
	move.w	#$100,$9A(A0)
	bra	e1c1

e1c0:
	cmp.w	#10,_chan+2
	bgt	e1c1
	clr.w	_eff+2
	move.w	#1,$B4(A0)

e1c1:
	move.w	#$100,$9C(A0)
	move.l	(sp)+,a4

#endasm
}

void EffectInt2 ()
{
#asm

	move.l	a4,-(sp)
	jsr	_geta4#

	tst.w	_eff+4
	bne	e2c0
	clr.w	_chan+4
	move.w	#$200,$9A(A0)
	bra	e2c1

e2c0:
	cmp.w	#10,_chan+4
	bgt	e2c1
	clr.w	_eff+4
	move.w	#1,$C4(A0)

e2c1:
	move.w	#$200,$9C(A0)
	move.l	(sp)+,a4

#endasm
}

void EffectInt3 ()
{
#asm

	move.l	a4,-(sp)
	jsr	_geta4#

	tst.w	_eff+6
	bne	e3c0
	clr.w	_chan+6
	move.w	#$400,$9A(A0)
	bra	e3c1

e3c0:
	cmp.w	#10,_chan+6
	bgt	e3c1
	clr.w	_eff+6
	move.w	#1,$D4(A0)

e3c1:
	move.w	#$400,$9C(A0)
	move.l	(sp)+,a4

#endasm
}

init_int4 ()
{
register int i;

	for( i = 0; i < 4; i ++ ) {
		EffectInterrupt[i].is_Node.ln_Type = NT_INTERRUPT;
		EffectInterrupt[i].is_Node.ln_Pri  = 0;
		EffectInterrupt[i].is_Node.ln_Name = NULL;
		EffectInterrupt[i].is_Data = 0;
	}
	EffectInterrupt[0].is_Code = EffectInt0;
	EffectInterrupt[1].is_Code = EffectInt1;
	EffectInterrupt[2].is_Code = EffectInt2;
	EffectInterrupt[3].is_Code = EffectInt3;

	for( i = 0; i < 4; i ++ ) {
		SetIntVector( INTB_AUD0+i, &EffectInterrupt[i] );
		eff [i] = chan [i] = 0;
	}
}

cycle_col ()
{
register int c;
static int coldir = 1, colcounter = 0;
static int coldir2 = 1, colcounter2 = 0;
static int colcounter3 = 0, count = 0;

	if (++count > 4) {
		count = 0;
		*colptr = cyccol [level][colcounter3][0];
		*(colptr+2) = cyccol [level][colcounter3][1];
		*(colptr+4) = cyccol [level][colcounter3][2];
		*(colptr+6) = cyccol [level][colcounter3][3];
		++colcounter3;
		colcounter3 &= 3;
	}

	if (coldir > 0) ++colcounter;
	else --colcounter;
	if (colcounter > 2*15) coldir = -1;
	if (colcounter < 1) coldir = 1;
	if (level > 3 && level < 7) *(colptr+8) = ((colcounter >> 1) << 4);
	else if (level == 7) *(colptr+8) = (colcounter >> 1);
	else *(colptr+8) = ((colcounter >> 1) << 8);

	if (coldir2 > 0) ++colcounter2;
	else --colcounter2;
	if (colcounter2 > 2*15) coldir2 = -1;
	if (colcounter2 < 2*5) coldir2 = 1;
	c = (colcounter2 >> 1);
	*(colptr2-2) = ((c * 0x100) & statr) | ((c * 0x010) & statg) | (c & statb);
}

set_obj ( nr, x, y )
register int nr, x, y;
{
	matrix[y][x] = (matrix[y][x] & 0xC7FF) | nr;
	if( x-(xpos>>4) >= 0 && x-(xpos>>4) < 20 ) {
		setobjx = x;
		setobjy = y;
	}
}

clr_obj ( x, y )
register int x, y;
{
	matrix[y][x] &= 0xC7FF;
	if( x-(xpos>>4) >= 0 && x-(xpos>>4) < 20 ) {
		setobjx = x;
		setobjy = -y;
	}
}

remake_ground (nr, x, y)
register int nr, x, y;
{
	matrix [y][x] = nr;
	if( x >= (xpos>>4) && x-(xpos>>4) < 20 ) {
		nr = x-(xpos>>4)+((xpos%336)>>4);
		copy_ground( x, y, nr);
		if( nr+21 < 40 ) copy_ground( x, y, nr+21);
		if( nr-21 >= 0 ) copy_ground( x, y, nr-21);
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
				copy_ground((xpos+320)>>4, y, (x + 320)>>4);
				copy_obj((xpos+320)>>4, y, (x + 320)>>4);
			}
			if(x > 15) {
				copy_ground((xpos+320)>>4, y, (x - 16)>>4);
				copy_obj((xpos+320)>>4, y, (x - 16)>>4);
			}
		}
	} else if( xdiff < 0 ) {
		y = xpos & 15;
		if( y <= 11 ) {
			if(x < 320 && xpos > 15) {
				copy_ground((xpos-16)>>4, y, (x + 320)>>4);
				if( y <= 10 ) copy_obj((xpos-16)>>4, y+1, (x + 320)>>4);
			}
			if(x > 15) {
				copy_ground((xpos-16)>>4, y, (x - 16)>>4);
				if( y <= 10 ) copy_obj((xpos-16)>>4, y+1, (x - 16)>>4);
			}
		}
	}

	xdiff = myxd;
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

DrawBobs ()
{
	if( bobon ) {
		boblisty = boby;
		boblistx = bobx;
		boblisti = bobimage;
	} else boblisti = 0;

/*	if( bobon ) *wptr = ((boblisty + (*boblisti & 0xFF) + 0x3F) << 8) | 1;
*/
}

init_int2 ()
{
	BlitterInt.is_Node.ln_Type = NT_INTERRUPT;
	BlitterInt.is_Node.ln_Pri  = 0;
	BlitterInt.is_Node.ln_Name = NULL;
	BlitterInt.is_Data = 0;
	BlitterInt.is_Code = blitter_int;
	BlitterIntOld = SetIntVector (INTB_BLIT, &BlitterInt);
}

init_int3 ()
{
	CompInterrupt.is_Node.ln_Type = NT_INTERRUPT;
	CompInterrupt.is_Node.ln_Pri  = 0;
	CompInterrupt.is_Node.ln_Name = "CompInt";
	CompInterrupt.is_Data = 0;
	CompInterrupt.is_Code = computer_int;

	ccount = -1;
	AddIntServer( INTB_VERTB, &CompInterrupt );
}

void computer_int ()
{
register long i;
int x1, x2, y1, y2;

#asm
	movem.l a0-a6/d0-d7,-(sp)
	jsr _geta4#
#endasm

	if( ccount > -1 ) {
		for( i = ccount; i < ccount + cnum; i ++ ) {
			if( i >= clength ) unplot( x1 = (int)(32L*sintable[(xmul*(i-clength))%360] >> 10), y1 = (12*sintable[(ymul*(i-clength))%360] >> 10) );
			plot( x2 = (int)(32L*sintable[(xmul*i)%360] >> 10), y2 = (12*sintable[(ymul*i)%360] >> 10) );
			if( i >= clength ) unplot( x1, -y1 );
			plot( x2, -y2 );
		}
		ccount = i;
	}
	if( title_on != 0 ) {
		if ( title_on > 10 ) {
			*(colptr-10*2) = titable[tii];
			*(colptr-8*2)  = titable[(tii+1)%3];
			*(colptr+4*2)  = titable[(tii+2)%3];
			tii ++;
			tii %= 3;
			title_on = 1;
		} else title_on ++;
	}
;
#asm
	movem.l (sp)+,a0-a6/d0-d7
	moveq.l	#0,D0
#endasm

}

void blitter_int ()
{
register int i, j;
register char *ptr;
register LONG *wptr;

#asm
	movem.l a0-a6/d0-d7,-(sp)
	jsr _geta4#
#endasm

	i = akbob;
	if( i == -1 && !clrlistdx ) i = akbob = 1;
	if( i == 1 && !boblisti ) i = akbob = 0;
	if( i == -1 ) {
		copy_bob_bp( clrlistx, clrlisty, clrlistdx, clrlistdy );
		if( akplane < MAXPLANES-1 ) akplane ++;
		else {
			if( boblisti ) akbob = 1;
			else akbob = 0;
			akplane = 0;
			clrlistdx = 0;
		}
	} else if( akbob == 1 ) {
		if( akplane == 0 ) {
			clrlistx = boblistx + xscreen;
			clrlisty = boblisty;
			ptr = (char *)boblisti;
			clrlistdx = *ptr + 1;
			ptr ++;
			clrlistdy = *ptr;
			if( clrlisty + clrlistdy > 192 ) clrlistdy = 192 - clrlisty;

			if( setobjy > 0 ) {
				j = setobjx-(xpos>>4)+((xpos%336)>>4);
				copy_obj( setobjx, setobjy, j);
				if( j + 21 < 40 ) copy_obj( setobjx, setobjy, j+21);
				if( j - 21 >= 0 ) copy_obj( setobjx, setobjy, j-21);
			}
			else if( setobjy < 0 ) {
				setobjy = -setobjy;
				j = setobjx-(xpos>>4)+((xpos%336)>>4);
				copy_ground( setobjx, setobjy, j);
				copy_ground( setobjx, setobjy-1, j);
				if( j + 21 < 40 ) {
					copy_ground( setobjx, setobjy, j+21);
					copy_ground( setobjx, setobjy-1, j+21);
				}
				if( j - 21 >= 0 ) {
					copy_ground( setobjx, setobjy, j-21);
					copy_ground( setobjx, setobjy-1, j-21);
				}
			}
			setobjy = 0;
		}
		copy_bp_bob ( clrlistx, clrlisty, clrlistdx, clrlistdy );
		if( akplane < MAXPLANES-1 ) akplane ++;
		else {
			akbob = 2;
			akplane = 0;
		}
	}
	else if( akbob == 2 ) {
		blit_bob_bp ( boblisti, boblistx, boblisty );
		if( akplane < MAXPLANES-1 ) akplane ++;
		else {
			akbob = 0;
			akplane = 0;
		}
	}
	else if( akbob == 0 && sinny ) {
		copy_sinn( sinnx, sinny, sinnx-(xpos>>4)+((xpos%336)>>4));
		if( specmv != SMLIFTUP && specmv != SMLIFTDN ) sinny = 0;
	}

	custom.intreq = INTF_BLIT;
	;
#asm
	movem.l (sp)+,a0-a6/d0-d7
	moveq.l	#0,D0
#endasm
}

add_sprite ()
{
register int i;

	do_sound (10);
	for( i = 0; vspriteon[i]; i ++ );
	vspriteon[i] = 1;
	slist[sprites] = i;
	sprites ++;

	return i;
	enigma4 ();
}

rem_sprite ( nr )
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
