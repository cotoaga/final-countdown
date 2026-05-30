extern struct CompDef {
	int x, y, typ, files, accessmode [MAXDEVS];
} comp [MAXCOMPS];

extern struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS];

extern struct Sound {
	long start, length;
	int speed, vol, pri;
} sound [], syssound [];

extern LONG BitPlaneA [MAXPLANES];
extern UWORD lomem;

extern LONG foex [MAXFOES], foetxl [MAXFOES], foetxr [MAXFOES];
extern LONG ttime, itime, dietime, mxp, score, gtime;
extern WORD leveladd, justdied;
extern WORD habeilev [MAXLEVELS];
extern WORD fueladd, collision, master, fuel, countdown, minex, miney;
extern WORD tacdes, steexc, specmv, xp, yp, message, statr, statg, statb, no, expt, expfn;
extern WORD seldes, viccol, foes, foev [MAXFOES], foed [MAXFOES], foef [MAXFOES];
extern WORD trans, green, hygkoo, tarpen, gorg, fatal, level, xray, intal, novbom, chan [4], eff [4];
extern UWORD xpos, *font, *computer;
extern UWORD *clears, matrix [15][128];
extern UWORD *colorptr, colortable [32];

LONG lutzs, lutzd;
WORD button, joyx, joyy;
WORD esc, disk, path, comps, devs, devstatus [MAXDEVS];
WORD devicestatus [MAXLEVELS][MAXDEVS];
LONG robotstatus [MAXLEVELS][MAXFOES];
BYTE *buttonptr = 0xbfe001;

LONG mestime [11] = {
	0, 100, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500
};

set_red ()
{
	statr = 15<<8;
	statg = 0;
	statb = 0;
}

set_blue ()
{
	statr = 0;
	statg = 0;
	statb = 15;
}

set_green ()
{
	statr = 0;
	statg = 15<<4;
	statb = 0;
}

set_yellow ()
{
	statr = 15<<8;
	statg = 15<<4;
	statb = 0;
}

set_status_col ()
{
	set_green ();
	if (itime >= 8000) set_yellow ();
	if (novbom || level == 3 || green) set_red ();
	if (message > 0) set_blue ();
}


radiation ()
{
register int i, j, k;
static int add, r;


	add ++;
	add &= 63;
	if( add < 32 ) j = add >> 3;
	else j = 7 - (add >> 3);

	if( r <= 0 ) r --;
	if( r < - 6 ) r = (rand () & 0x07) + 1;
	if( r > 0 ) {
		for( i = 1; i < 32; i ++ ) {
			if( i == 11 ) i = 16;
			k = (colortable[i] >> 8) + j + 2;
			if( k > 15 ) k = 15;
			*(colorptr+(i<<1)) = (colortable[i] & 0xFF) | (k << 8);
		}
	}
	if( add == 31 ) {
		r --;
		if( r == 0 ) {
			for( i = 1; i < 32; i ++ ) {
				*(colorptr+(i<<1)) = 0xFFF;
			}
		}
	}

}

no_name ()
{
register int i, j;
register long var;

	++gtime;
	if (level == 3) radiation ();
	if ((var = (gtime >> 6)) != score) {
		score = var;
		new_score ();
		if (ttime == score) trans = 1;
		if (level == 3) {
			++xray;
			do_sound (18);
			if (xray == 100) {
				collision = 2;
				if (!master) specmv = SMDIE;
			}
		}
		if (green) {
			++green;
			do_sound (18);
			if (green == 30) {
				collision = 2;
				if (!master) specmv = SMDIE;
			}
		}
		if (intal) {
			++intal;
			set_yellow ();
			if (intal == 10) {
				do_sound (2);
				do_sound (2);
				itime = 9998;
				fatal = 5;
			}
		}
		if (tarpen) {
			++tarpen;
			if (tarpen == 20) {
				if (itime < 9800) itime = 9800;
				do_sound (7);
				light_puls ();
				viccol = 1;
				tarpen = 0;
			}
		}
		if (viccol) {
			++viccol;
			if (viccol == 20) {
				hygkoo = 1;
				do_sound (8);
				light_off ();
			}
		}
		if (itime) {
			++itime;
			if (itime == 10000) {
				novbom = 1;
				set_red ();
				do_sound (7);
				do_sound (8);
				do_sound (13);
				flash ();
			}
		}
		if (novbom) {
			++novbom;
			if (novbom == 30) apokalypse ();
		}
		if (steexc) {
			++steexc;
			if (steexc == 200) apokalypse ();
		}
		if (seldes) {
			++seldes;
			if (seldes == 200) apokalypse ();
		}
		for (i = 1; i < 11; i++) if (score == mestime [i]) {
			message = i;
			do_sound (6);
			set_blue ();
			break;
		}
	}
	if (fueladd) {
		if (fuel < 1000) {
			--fueladd;
			fuel += 2;
			disp_fuel ();
		} else fueladd = 0;
	}
	if (expfn != -1) {
		if (expt > 1) --expt;
		else {
			kill_foe (expfn);
			expfn = -1;
		}
	}

	if (countdown)
		if (!(--countdown)) {
			set_obj (MTRAP, minex, miney);
			do_sound (2);
			do_sound (2);
		}

	refresh_mines ();
	refresh_score ();
}

store_devices (level)
int level;
{
register int i;

	habeilev [level] = 1;
	for (i = 0; i < devs; i++)
		devicestatus [level][i] = devstatus [i];
	for (i = 0; i < foes; i++)
		robotstatus [level][i] = foex [i];
}

restore_devices (level)
int level;
{
register int i, x, y;
int n, f;

	if (habeilev [level]) {
		for (i = 0; i < foes; i++)
			foex [i] = robotstatus [level][i];
		for (i = 0; i < devs; i++) {
			devstatus [i] = devicestatus [level][i];
			n = device [i].next; device [i].next = 0;
			f = device [i].spec; if (device [i].typ != MFOE) device [i].spec = 0;
			if (devstatus [i]) dev_on (i);
			else dev_off (i);
			device [i].next = n;
			device [i].spec = f;
		}
		if (!justdied) for (x = 0; x < 128; x++)
			for (y = 0; y < 12; y++) {
				if ((matrix [y][x] & CHECKTHING) == MMINE) matrix [y][x] &= 0xC7FF;
		}
		if (level == 2) foed [1] = foed [0] = 0;
	} else for (i = 0; i < devs; i++) devstatus [i] = device [i].on;
	if (gorg) for (i = 0; i < foes; i++) if (foef [i] == 2) foed [i] = 0;

	if ((1 << level) & disk)
		for (x = 0; x < 128; x++)
			for (y = 0; y < 12; y++)
				if ((matrix [y][x] & CHECKTHING) == MDISK1) matrix [y][x] &= 0xC7FF;

	if (level == 6 && tacdes) {
		matrix [3][121] = 56 | MGROUND;
		matrix [3][122] = 85;
		matrix [3][123] = 85;
		matrix [3][124] = 85;
		matrix [3][125] = 51 | MGROUND;
	}
}

use_dev ()
{
register int i, x, y;
register long ii;

	y = yp >> 10;
	x = mxp >> 9;
	for (no = 0; no < comps; no++)
		if ((x == comp [no].x || x-1 == comp [no].x || x+1 == comp [no].x) && y == comp [no].y)
			break;

	if (no == comps) {
		return;
		enigma2 ();
	}

	if (comp [no].typ == MCOMPUTER) use_computer ();
	else for (i = 0; i < devs; i++) if (comp [no].accessmode [i] == 3) handle_dev (i);
}

handle_dev (no)
int no;
{
register int x, y, f, n;

	x = device [no].x;
	y = device [no].y;
	f = device [no].spec;
	n = device [no].next;

	if (device [no].typ == MENERGY) {
		if (devstatus [no]) {
			remake_ground ((MFLASH+6), x, y);
			remake_ground ((MFLASH+7), x+1, y);
			remake_ground ((MFLASH+8), x, y+1);
			remake_ground ((MFLASH+9), x+1, y+1);
			remake_ground ((MFLASH+10), x, y+2);
			remake_ground ((MFLASH+11), x+1, y+2);
			remake_ground ((MGFLASH+2) | MGROUND, x, y+3);
			remake_ground ((MGFLASH+3) | MGROUND, x+1, y+3);
			devstatus [no] = 0;
			do_sound (14);
			if (f) {
				if (device [no].xl1 < foetxl [f]) foetxl [f] = device [no].xl1;
				if (device [no].xr2 > foetxr [f]) foetxr [f] = device [no].xr2;
			}
			if (n) handle_dev (n);
		} else {
			remake_ground ((MFLASH+0)|MENERGY, x, y);
			remake_ground ((MFLASH+1)|MENERGY, x+1, y);
			remake_ground ((MFLASH+2)|MENERGY, x, y+1);
			remake_ground ((MFLASH+3)|MENERGY, x+1, y+1);
			remake_ground ((MFLASH+4)|MENERGY, x, y+2);
			remake_ground ((MFLASH+5)|MENERGY, x+1, y+2);
			remake_ground ((MGFLASH+0) | MGROUND|MENERGY, x, y+3);
			remake_ground ((MGFLASH+1) | MGROUND|MENERGY, x+1, y+3);
			devstatus [no] = 1;
			do_sound (13);
			if (f) {
				if (foef [f] & 4) x = 2*32*16;
				else x = 0;
				if (foex [f] > device [no].xl2 - x) {
					if (foex [f] < device [no].xr2) foetxl [f] = device [no].xl2;
				} else if (foex [f] < device [no].xr1 + x) {
					if (foex [f] > device [no].xl1) foetxr [f] = device [no].xr1;
				} else {
					kill_foe (f);
					do_sound (11);
				}
			}
		}
	} else if (device [no].typ == MAGRAV) {
		if (devstatus [no]) {
			remake_ground (MLIFT+9, x, y);
			remake_ground (MLIFT+10, x+1, y);
			remake_ground (MLIFT+11, x+2, y);
			remake_ground (MLIFT+12, x, y+1);
			remake_ground (MLIFT+13, x+1, y+1);
			remake_ground (MLIFT+14, x+2, y+1);
			remake_ground (MLIFT+15, x, y+2);
			remake_ground (MLIFT+16, x+1, y+2);
			remake_ground (MLIFT+17, x+2, y+2);
			remake_ground ((MAGRAV+3) | MGROUND, x, y+3);
			remake_ground ((MAGRAV+4) | MGROUND, x+1, y+3);
			remake_ground ((MAGRAV+5) | MGROUND, x+2, y+3);
			do_sound (14);
			devstatus [no] = 0;
			if (n) handle_dev (n);
		} else {
			remake_ground (MLIFT+0, x, y);
			remake_ground (MLIFT+1, x+1, y);
			remake_ground (MLIFT+2, x+2, y);
			remake_ground (MLIFT+3, x, y+1);
			remake_ground (MLIFT+4, x+1, y+1);
			remake_ground (MLIFT+5, x+2, y+1);
			remake_ground (MLIFT+6, x, y+2);
			remake_ground (MLIFT+7, x+1, y+2);
			remake_ground (MLIFT+8, x+2, y+2);
			remake_ground ((MAGRAV+0) | MGROUND, x, y+3);
			remake_ground ((MAGRAV+1) | MGROUND, x+1, y+3);
			remake_ground ((MAGRAV+2) | MGROUND, x+2, y+3);
			devstatus [no] = 1;
			do_sound (13);
			if (n) handle_dev (n);
		}
	} else {
		if (devstatus [no]) {
			do_sound (14);
			foed [f] = devstatus [no] = 0;
			if (n) handle_dev (n);
		} else {
			do_sound (13);
			foed [f] = 1;
			devstatus [no] = 1;
			if (n) handle_dev (n);
		}
	}
}

joystick ()
{
	joyy = (-(((custom.joy1dat & 0x100) != NULL) ^ ((custom.joy1dat & 0x200) != NULL)));
	joyy = (joyy < 0)? -1: (((custom.joy1dat & 2) != 0) ^ ((custom.joy1dat & 1) != 0));
	joyx = (-((custom.joy1dat & 0x200) != NULL));
	joyx = (joyx < 0)? -1: ((custom.joy1dat & 2) != NULL);
	button = ((*buttonptr & 0x80) == 0);

	if (collision && !master) joyy = joyx = button = 0;
}

make_computer ()
{
	lutzs = (long) computer;
	lutzd = (long) BitPlaneA[0];
	decompd ();
}

make_display ()
{
register int x, y, i, j;

	i = (xpos % 336) >> 4;
	j = xpos >> 4;

	for( x = 0; x < 20; x ++ ) {
		for( y = 0; y < 12; y ++ ) {
			if( x + i + 21 < 40 ) {
				copy_ground( j + x, y, x + i + 21);
				copy_obj( j + x, y, x + i + 21);
			}
			if( x + i - 21 >= 0 ) {
				copy_ground( j + x, y, x + i - 21);
				copy_obj( j + x, y, x + i - 21);
			}
		}
	}
	for( x = -1; x < 21; x ++ ) {
		for( y = 0; y < 12; y ++ ) {
			if( x + i >= 0 && x + i < 40 ) {
				copy_ground( j + x, y, x + i );
				copy_obj( j + x, y, x + i );
			}
		}
	}
	for( y = 0; y <= (int)(xpos & 15) - 4; y ++ ) {
		if( i >= 1 ) {
			copy_ground( j + 20, y, i - 1 );
			copy_obj( j + 20, y, i - 1 );
		}
	}

	for( y = 11; y >= (int)(xpos & 15); y -- ) {
		if( i < 20 ) {
			copy_ground( j - 1, y, i + 20 );
			if( y <= 10 ) copy_obj( j - 1, y+1, i + 20 );
		}
	}
}

put_line (ptr)
register char *ptr;
{
register int i;

	for( i = 0; *(ptr+i) && !(*(ptr+i)==0x0A); i ++ ) put_char( ascii( *(ptr+i) ), i, 12);
	scroll_screen ();
	for (i = 0; i < 20000; i++);
}

put_points ()
{
register int i, j;

	for (i = 0; i < 5; i++) {
		put_char (ascii( '.'), i, 12);
		do_sound (0);
		for (j = 0; j < 30000; j++);
	}
	scroll_screen ();
}

put_start_line (ptr)
register char *ptr;
{
register int i;

	for( i = 0; *(ptr+i) && !(*(ptr+i)==0x0A); i ++ ) put_char( ascii( *(ptr+i) ), i, 12);
	scroll_screen ();
}

scroll_screen ()
{
register int i;

	for( i = 0; i < MAXPLANES; i ++ ) {
		WaitBlit ();
		custom.bltdpt   = (APTR) (BitPlaneA[i] + 80*9 + 12);
		custom.bltcpt   = (APTR) (BitPlaneA[i] + 80*17 + 12);
		custom.bltcmod  = 40;
		custom.bltdmod  = 40;
		custom.bltcon1  = 0;
		custom.bltcon0  = DEST | SRCC | 0xAA;
		custom.bltsize  = 20 + (96 << 6);
	}
	for( i = 0; i < 100; i ++ );
	WaitBlit ();
}

char *get_line ()
{
static char ptr[41];
register int i, j;

	i = 0;
	while( i < 40 ) {
		put_char ( 59, i, 12 );
		j = key ();
		if( j < 44 && i < 39 ) {
			ptr[i] = j;
			if( j != 43 ) put_char( j, i, 12 );
			else put_char( 0, i, 12 );
			i ++;
		} else if( j == 44 && i > 0 ) {
			put_char( 0, i, 12 );
			i --;
			ptr[i] = 0;
		} else if( j == 45 ) {
			ptr[i] = 0;
			put_char( 0, i, 12 );
			i = 40;
		}
	}
	scroll_screen ();
	for( i = 0; ptr[i] != 0; i ++ ) ptr[i] = asc( ptr[i] );

return ptr;
}

asc( nr )
register int nr;
{
	if( nr > 0x00 && nr <= 0x0A ) nr += 0x2F;
	else if( nr >= 0x0B && nr < 0x25  ) nr += 0x36;
	else if( nr == 43 ) nr = 0x20;

return nr;
}

ascii( nr )
register int nr;
{
	if( nr >= 0x30 && nr < 0x3A ) nr -= 0x2F;
	else if( nr >= 0x41 && nr < 0x5B  ) nr -= 0x36;
	else if( nr == 37 ) nr += 2;
	else if( nr >= 33 && nr < 36 ) nr += 4;
	else if( nr >= 44 && nr < 48 ) nr -= 4;
	else if( nr == 58 ) nr = 44;
	else if( nr == 59 ) nr = 45;
	else if( nr == 63 ) nr = 46;
	else if( nr == 61 ) nr = 47;
	else if( nr == 43 ) nr = 48;
	else if( nr == 61 ) nr = 38;
	else if( nr == 0x20 ) nr = 0;
	else if( nr >= 0x61 ) nr -= 0x30;

return nr;
}

put_char (nr, x, y)
register int nr, y;
int x;
{
register char *ptrS, *ptrD;
register int j, i;

	ptrS = (char *) font + nr;
	for( i = 0; i < MAXPLANES; i ++ ) {
		ptrD = (char *)BitPlaneA[i] + x + 640*y + 92;
		for( j = 0; j < 8; j ++ ) {
			*ptrD = *ptrS;
			ptrD += 80;
			ptrS += 60;
		}
	}
}

char keycode[] = {
0x7F, 0xEB, 0xFD, 0xFB, 0xF9, 0xF7, 0xF5, 0xF3, 0xF1, 0xEF,
0xED, 0xBF, 0x95, 0x99, 0xBB, 0xDB, 0xB9, 0xB7, 0xB5, 0xD1,
0xB3, 0xB1, 0xAF, 0x91, 0x93, 0xCF, 0xCD, 0xDF, 0xD9, 0xBD,
0xD7, 0xD3, 0x97, 0xDD, 0x9B, 0x9D, 0xD5, 0x8D, 0x7D, 0x77 };
char *addr = 0xBFEC01;

char *shortkey[] = {
"ENABLE ",
"DISABLE ",
"TYPE ",
"SHOW ",
"LIST",
"DIR ",
"GATE ",
"LIFT ",
"SYSTEM",
"ENDCLI",
"HELP "
};

key ()
{
register char c;
register int i;
static char lastc;
static char wword, wnum;

	if( !wword ) {
		do {
			i = 40;
			c = *addr;
			if( c == lastc ) continue;
			lastc = c;
			for( i = 0; i < 40; i ++ ) if( keycode[i] == c ) break;
			wword = 48 - (c >> 1);
			if( wword > 0 && wword <= 10 && (c & 1) ) wnum = i = 0;
			else wword = 0;
			if( c == 0x41 ) wword = 11, wnum = i = 0;
		} while ( i == 40 );
		if( i >= 37 ) i += 6;
		if( i == 0 ) i = 43;
		do_sound (18);
	}
	if( wword ) {
		i = ascii( *(shortkey[wword-1] + wnum) );
		if( *(shortkey[wword-1] + wnum) == 0x20 ) i = 43;
		wnum ++;
		if( *(shortkey[wword-1] + wnum) == 0 ) wword = 0;
	}

/* 43 = SPACE, 44 = DELETE, 45 = RETURN */

return i;
}

fkey ()
{
register unsigned char c;
register int i;
static char lastc;

	i = -1;
	do {
		c = *addr;
		if( c == lastc ) continue;
		lastc = c;
		if( c == 0x7F ) i = 10;
		if( (c & 1) && i != 10 ) i = 47 - (c >> 1);
	} while ( i < 0 || i > 10 );
	do_sound (18);

return i;
}

test_key ()
{
char c;
register int i;

	c = *addr;
	if( c == 0x75 && !collision ) {
		for (i = 0; i < 32000; i++);
		if (!master) collision = esc = 1;
		master = 0;
	}
}

plot( x, y )
register UWORD x, y;
{
register UWORD i;
register UWORD *ptr;

	y += 12;
	x += 9 + 32;
	i = ((x >> 3) & 0xFFFE) + y * 80 + 42*80 + 58;/*28*/
	x &= 15;
	ptr = (UWORD *)(BitPlaneA[0] + i);
	*ptr |= 0x8000 >> x;
	ptr = (UWORD *)(BitPlaneA[1] + i);
	*ptr &= 0xFFFF - (0x8000 >> x);
	ptr = (UWORD *)(BitPlaneA[2] + i);
	*ptr |= 0x8000 >> x;
	ptr = (UWORD *)(BitPlaneA[3] + i);
	*ptr |= 0x8000 >> x;
}

unplot( x, y )
register UWORD x, y;
{
register int i;
register UWORD *ptr;

	y += 12;
	x += 9 + 32;
	i = ((x >> 3) & 0xFFFE) + y * 80 + 42*80 + 58;
	x &= 15;
	ptr = (UWORD *)(BitPlaneA[0] + i);
	*ptr &= 0xFFFF - (0x8000 >> x);
	ptr = (UWORD *)(BitPlaneA[1] + i);
	*ptr |= 0x8000 >> x;
	ptr = (UWORD *)(BitPlaneA[2] + i);
	*ptr &= 0xFFFF - (0x8000 >> x);
	ptr = (UWORD *)(BitPlaneA[3] + i);
	*ptr &= 0xFFFF - (0x8000 >> x);
}

kill_chan_with_pri (pri)
int pri;
{
register int i, ii;

	for (i = 0; i < 4; i++) if (chan [i] == pri) {
		custom.dmacon = DMAF_AUD0 << i;
		chan [i] = eff [i] = 0;
		for (ii = 0; ii < 300; ii++);
		custom.aud [i].ac_len = 10;
		custom.aud [i].ac_vol = 0;
		custom.aud [i].ac_per = 200;
		custom.dmacon = DMAF_SETCLR | (DMAF_AUD0 << i) | DMAF_AUDIO;
		custom.intena = INTF_SETCLR | (INTF_AUD0 << i);
	}
}


do_sound (nr)
register int nr;
{
register int i, pri, chn;

	if( lomem ) return;

	for (chn = 1, i = 2; i < 4; i++) if (chan [i] < chan [chn]) chn = i;
	if (nr == 4) chn = 0;

	if (sound [nr].pri == 3 && (chan [1] == 3 || chan [2] == 3 || chan [3] == 3)) return;

	if( nr >= 0 ) {
		if (chan [chn] <= sound [nr].pri) {
			custom.dmacon = DMAF_AUD0 << chn;
			for (i = 0; i < 300; i++);
			custom.aud [chn].ac_ptr = (UWORD *) sound [nr].start;
			custom.aud [chn].ac_len = sound [nr].length >> 1;
			custom.aud [chn].ac_vol = sound [nr].vol;
			custom.aud [chn].ac_per = sound [nr].speed;
			chan [chn] = sound [nr].pri;
			eff [chn] = 1;
			custom.dmacon = DMAF_SETCLR | (DMAF_AUD0 << chn) | DMAF_AUDIO;
			custom.intena = INTF_SETCLR | (INTF_AUD0 << chn);
		}
	} else {
		nr = -1 - nr;
		if (chan [chn] <= sound [nr].pri) {
			custom.dmacon = DMAF_AUD0 << chn;
			for (i = 0; i < 300; i++);
			custom.aud [chn].ac_ptr = (UWORD *) syssound [nr].start;
			custom.aud [chn].ac_len = syssound [nr].length >> 1;
			custom.aud [chn].ac_vol = syssound [nr].vol;
			custom.aud [chn].ac_per = syssound [nr].speed;
			chan [chn] = syssound [nr].pri;
			eff [chn] = 1;
			custom.dmacon = DMAF_SETCLR | (DMAF_AUD0 << chn) | DMAF_AUDIO;
			custom.intena = INTF_SETCLR | (INTF_AUD0 << chn);
		}
	}
}

apokalypse ()
{
register int i, j;

	light_end ();
	for (i = 0; i < 4; i++) {
		custom.dmacon = DMAF_AUD0 << i;
		custom.aud [i].ac_ptr = clears;
		custom.aud [i].ac_len = 5;
		custom.aud [i].ac_vol = 0;
		custom.aud [i].ac_per = 666;
		chan [i] = eff [i] = 1;
		for (j = 0; j < 666; j++);
		custom.dmacon = DMAF_SETCLR | (DMAF_AUD0 << i) | DMAF_AUDIO;
		custom.intena = INTF_SETCLR | (INTF_AUD0 << i);
	}
	master = 0;
	collision = 2;
	specmv = SMDIE;
}
