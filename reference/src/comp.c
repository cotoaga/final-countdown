extern struct Sound {
	long start, length;
	int speed, vol, pri;
} sound [];

extern struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS];


extern LONG lines[45];
extern LONG BitPlaneA [MAXPLANES], ccount;
extern LONG foex [MAXFOES], foetxl [MAXFOES], foetxr [MAXFOES];
extern WORD foed [MAXFOES], foef [MAXFOES], devstatus [MAXDEVS];
extern WORD level, statr, statg, statb, myxd, button, title_on, dark, akbob;
extern WORD message, chan [4], eff [4], green;

extern WORD ekiman,tacdes, steexc, specmv;
extern WORD fatal, semcom, seldes, viccol;
extern WORD trans, green, hygkoo, tarpen, gorg, xray, intal, novbom;
extern WORD disk, mass, dka, dkb, dkc;

extern UWORD xpos, matrix [15][128];
extern UWORD *clears, *bpptr [MAXPLANES], *delayptr, *colorptr, *bpl0ptr, *hiptr;
extern UWORD bobon, loosercol [], syscolors [], compcolors [], colortable [];


/* Wasserstoff, Stickstoff, Sauerstoff, Fluor,  Chlor */
WORD ing[5];
char *gas[5] = { "H", "N", "O", "F", "CL" };
extern WORD air_bad;
WORD air_level[MAXLEVELS][5] = {
{0,8,2,0,0},
{0,8,2,0,0},
{0,8,2,0,0},
{0,8,2,0,0},
{0,8,2,0,0},
{0,8,2,0,0},
{0,8,2,0,0},
{0,0,0,7,3}
};

char airparts[5][15] =
{
"H  001  = 00 %",
"N  007  = 00 %",
"O  008  = 00 %",
"F  009  = 00 %",
"CL 017  = 00 %"
};
UWORD infected[8];

WORD no, zahl, lerror, procom, firsttime = 1;

BYTE *get_line (), *line, word1 [40], word2 [40], word3 [40];

WORD pcomno [MAXLEVELS] = {
	9, 10, 7, 2, 5, 1, 8, 11
};

dev_on (no)
int no;
{
register int x, y, f, n;

	x = device [no].x;
	y = device [no].y;
	f = device [no].spec;
	n = device [no].next;

	if (device [no].typ == MENERGY) {
		matrix [y][x] = (MFLASH+0)|MENERGY;
		matrix [y][x+1] = (MFLASH+1)|MENERGY;
		matrix [y+1][x] = (MFLASH+2)|MENERGY;
		matrix [y+1][x+1] = (MFLASH+3)|MENERGY;
		matrix [y+2][x] = (MFLASH+4)|MENERGY;
		matrix [y+2][x+1] = (MFLASH+5)|MENERGY;
		matrix [y+3][x] = (MGFLASH+0) | MGROUND|MENERGY;
		matrix [y+3][x+1] = (MGFLASH+1) | MGROUND|MENERGY;
		devstatus [no] = 1;
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
		if (n) {
			if (devstatus [n]) {
				dev_off (n);
				put_whwtd (device [n].typ, n, 0);
			} else {
				dev_on (n);
				put_whwtd (device [n].typ, n, 1);
			}
		}
	} else if (device [no].typ == MAGRAV) {
		matrix [y][x] = MLIFT+0;
		matrix [y][x+1] = MLIFT+1;
		matrix [y][x+2] = MLIFT+2;
		matrix [y+1][x] = MLIFT+3;
		matrix [y+1][x+1] = MLIFT+4;
		matrix [y+1][x+2] = MLIFT+5;
		matrix [y+2][x] = MLIFT+6;
		matrix [y+2][x+1] = MLIFT+7;
		matrix [y+2][x+2] = MLIFT+8;
		matrix [y+3][x] = (MAGRAV+0) | MGROUND;
		matrix [y+3][x+1] = (MAGRAV+1) | MGROUND;
		matrix [y+3][x+2] = (MAGRAV+2) | MGROUND;
		devstatus [no] = 1;
		if (n) {
			if (devstatus [n]) {
				dev_off (n);
				if (no + 1 != n) put_whwtd (device [n].typ, n, 0);
			} else {
				dev_on (n);
				if (no + 1 != n) put_whwtd (device [n].typ, n, 1);
			}
		}
	} else {
		foed [f] = 1;
		devstatus [no] = 1;
		if (n) {
			if (devstatus [n]) {
				dev_off (n);
				put_whwtd (device [n].typ, n, 0);
			} else {
				dev_on (n);
				put_whwtd (device [n].typ, n, 1);
			}
		}
	}
}

dev_off (no)
int no;
{
register int x, y, f, n;

	x = device [no].x;
	y = device [no].y;
	f = device [no].spec;
	n = device [no].next;

	if (device [no].typ == MENERGY) {
		matrix [y][x] = (MFLASH+6);
		matrix [y][x+1] = (MFLASH+7);
		matrix [y+1][x] = (MFLASH+8);
		matrix [y+1][x+1] = (MFLASH+9);
		matrix [y+2][x] = (MFLASH+10);
		matrix [y+2][x+1] = (MFLASH+11);
		matrix [y+3][x] = (MGFLASH+2) | MGROUND;
		matrix [y+3][x+1] = (MGFLASH+3) | MGROUND;
		devstatus [no] = 0;
		if (f) {
			if (device [no].xl1 < foetxl [f]) foetxl [f] = device [no].xl1;
			if (device [no].xr2 > foetxr [f]) foetxr [f] = device [no].xr2;
		}
		if (n) {
			if (devstatus [n]) {
				dev_off (n);
				put_whwtd (device [n].typ, n, 0);
			} else {
				dev_on (n);
				put_whwtd (device [n].typ, n, 1);
			}
		}
	} else if (device [no].typ == MAGRAV) {
		matrix [y][x] = MLIFT+9;
		matrix [y][x+1] = MLIFT+10;
		matrix [y][x+2] = MLIFT+11;
		matrix [y+1][x] = MLIFT+12;
		matrix [y+1][x+1] = MLIFT+13;
		matrix [y+1][x+2] = MLIFT+14;
		matrix [y+2][x] = MLIFT+15;
		matrix [y+2][x+1] = MLIFT+16;
		matrix [y+2][x+2] = MLIFT+17;
		matrix [y+3][x] = (MAGRAV+3) | MGROUND;
		matrix [y+3][x+1] = (MAGRAV+4) | MGROUND;
		matrix [y+3][x+2] = (MAGRAV+5) | MGROUND;
		devstatus [no] = 0;
		if (n) {
			if (devstatus [n]) {
				dev_off (n);
				if (no + 1 != n) put_whwtd (device [n].typ, n, 0);
			} else {
				dev_on (n);
				if (no + 1 != n) put_whwtd (device [n].typ, n, 1);
			}
		}
	} else if (f) {
		foed [f] = devstatus [no] = 0;
		if (n) {
			if (devstatus [n]) {
				dev_off (n);
				put_whwtd (device [n].typ, n, 0);
			} else {
				dev_on (n);
				put_whwtd (device [n].typ, n, 1);
			}
		}
	}
}

show_pic (/* name*/ )
{
char *name =  "gfx:Lost" ;
register int i, oldon;
register long j;
register UWORD *ptr;

	oldon = bobon;
	bobon = 0;
	while (akbob);
	DrawBobs ();
	akbob = -1;
	while (akbob);
	DrawBobs ();
	akbob = -1;

	custom.intena = INTF_BLIT;
	custom.dmacon = DMAF_RASTER | DMAF_SPRITE;

	load_colors( loosercol,0 );

	for( i = 0; i < MAXPLANES; i ++ ) {
		j = BitPlaneA[i] - 2;
		*bpptr[i]     = j >> 16;
		*(bpptr[i]+2) = j & 0xFFFF;
	}
	*delayptr = 0;

	load_pic( name );

	custom.dmacon = DMAF_SETCLR | DMAF_RASTER;

	do joystick ();
	while ( !button );

	ptr = (UWORD *)BitPlaneA[0];
	for( j = 0; j < MAXPLANES*WIDTHS*HEIGHT/16; j ++ ) *ptr ++ = 0;

	custom.dmacon = DMAF_SETCLR | DMAF_RASTER | DMAF_SPRITE;
	custom.intena = INTF_SETCLR | INTF_BLIT;
	custom.aud [0].ac_vol = 64;

	bobon = oldon;
}


title ()
{
register int i;
register long j;
register UWORD *ptr;

	bobon = 0;

	custom.intena = INTF_BLIT;
	custom.dmacon = DMAF_RASTER | DMAF_SPRITE;

	load_colors( syscolors,2 );

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

	*bpl0ptr = 0xC200;
	*hiptr = 0x3C;
	*(hiptr+2) = 0;
	*(hiptr+4) = 0;

	for( i = 0; i < MAXPLANES; i ++ ) {
		j = BitPlaneA[i] - 2;
		*bpptr[i]     = j >> 16;
		*(bpptr[i]+2) = j & 0xFFFF;
	}
	*delayptr = 0;

	custom.dmacon = DMAF_SETCLR | DMAF_RASTER;

	if (firsttime) {
		do_sound( -4 );

		syscomp_light ( 15 );
		for( j = 0; j < 250000; j ++ );
		do_sound( -1 );
		title_on = 1;
		for( j = 0; j < 300000; j ++ );
		*(colorptr+2*2) = 0xFFF;
		for( j = 0; j < 10000; j ++ );
		do_sound( -2 );
		for( i = 0; i <= 5; i ++ ) {
			*(colorptr+2*2) = i;
			for( j = 0; j < 30000; j ++ );
		}
		ccount = 0;

		put_start_text ();

		do joystick ();
		while ( button );
		do joystick ();
		while ( !button );

		ccount = -1;
		title_on = 0;

		syscomp_dark ();

		kill_chan_with_pri( 12 );
		firsttime = 0;
	}

	statr = statb = 0;
	statg = 15<<4;

	custom.dmacon = DMAF_RASTER;

	load_colors( colortable,2 );
	myxd = 0;
	ScrollBG ();

	*bpl0ptr = 0x5200;
	*hiptr = 0x38;
	*(hiptr+2) = 0x28;
	*(hiptr+4) = 0x28;

	ptr = (UWORD *)BitPlaneA[0];
	for( j = 0; j < MAXPLANES*WIDTHS*HEIGHT/16; j ++ ) *ptr ++ = 0;

	custom.dmacon = DMAF_SETCLR | DMAF_RASTER | DMAF_SPRITE;
	custom.intena = INTF_SETCLR | INTF_BLIT;
	bobon = 1;

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
}



finished ()
{
register int i;
register long j;
register UWORD *ptr;

	bobon = 0;

	custom.intena = INTF_BLIT;
	custom.dmacon = DMAF_RASTER | DMAF_SPRITE;

	load_colors( syscolors,2 );

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

	*bpl0ptr = 0xC200;
	*hiptr = 0x3C;
	*(hiptr+2) = 0;
	*(hiptr+4) = 0;

	for( i = 0; i < MAXPLANES; i ++ ) {
		j = BitPlaneA[i] - 2;
		*bpptr[i]     = j >> 16;
		*(bpptr[i]+2) = j & 0xFFFF;
	}
	*delayptr = 0;

	load_endtexte ();
	custom.dmacon = DMAF_SETCLR | DMAF_RASTER;

	do_sound( -4 );

	syscomp_light ( 0 );
	title_on = 1;
	*(colorptr+2*2) = 5;
	ccount = 0;

	do_sound (16);
	if (fatal) {
		put_line  (lines[0]);
		put_line  (lines[1] );
	}
	if (fatal == 1) put_line ( lines[2]);
	if (fatal == 2) put_line ( lines[3]);
	if (intal) {
		if (fatal == 5) {
			put_line ( lines[4] );
			put_line ( lines[5] );
			fatal = -1;
		} else {
			put_line ( lines[14] );
			put_line( lines[15] );
		}
	} else if (fatal == 5) put_line ( lines[9] );
	if (fatal == 3) put_line ( lines[6] );
	if (fatal == 4) {
		put_line ( lines[7] );
		put_line ( lines[8] );
	}
	if (xray >= 100) {
		put_line ( lines[10] );
		put_line( lines[11] );
	}
	if (green >= 30) {
		put_line ( lines[12] );
		put_line( lines[13] );
	}
	if (semcom || tarpen || viccol || steexc || ekiman || tacdes ) put_line ( lines[16] );
	if (tacdes) put_line ( lines[17] );
	if (viccol) {
		put_line ( lines[18] );
		put_line ( lines[19] );
	}
	if (steexc) put_line ( lines[20] );
	if (ekiman) put_line ( lines[21] );
	if (semcom) put_line ( lines[22] );
	if (semcom || tarpen || viccol || steexc || ekiman || tacdes ) put_line ( lines[23] );
	if (novbom) {
		put_line (lines[24]);
		put_line (lines[25]);
		put_line (lines[26]);
	}
	if (steexc && !ekiman) {
		put_line (lines[27]);
		put_line (lines[28]);
		put_line (lines[29]);
	}
	if (seldes && trans != 2) {
		put_line (lines[30]);
		put_line (lines[31]);
	}
	if (!semcom) {
		put_line (lines[32]);
		put_line (lines[33]);
	}
	if (trans == 2) {
		if (mass == 76 && dka == 3263 && dkb == 3608 && dkc == 4433) {
			if (disk != 0xFF) {
				put_line (lines[39]);
				put_line (lines[40]);
			}
		} else {
			put_line (lines[34]);
			if (mass != 76) {
				put_line (lines[35]);
				put_line (lines[36]);
			}
			if (dka != 3263 && dkb != 3608 && dkc != 4433) {
				put_line (lines[37]);
				put_line (lines[38]);
			}
		}
	}
	if (seldes && semcom && trans == 2 && mass == 76 && dka == 3263 && dkb == 3608 && dkc == 4433) {
		put_line (lines[41]);
		put_line (lines[42]);
	}
	put_line (lines[43]);
	put_line (lines[44]);
	kill_chan_with_pri (13);

	do joystick ();
	while ( button );
	do joystick ();
	while ( !button );

	do_sound (17);
	clr_comp ();
	load_riss ("DISK01:TXT/PicFFF");
	kill_chan_with_pri (14);

	do joystick ();
	while ( button );
	do joystick ();
	while ( !button );

	ccount = -1;
	title_on = 0;

	syscomp_dark ();

	kill_chan_with_pri( 12 );

	statr = statb = 0;
	statg = 15<<4;

	custom.dmacon = DMAF_RASTER;

	*bpl0ptr = 0x5200;
	*hiptr = 0x38;
	*(hiptr+2) = 0x28;
	*(hiptr+4) = 0x28;

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
}

use_computer ()
{
register int i;
register long j;
register UWORD *ptr;

	bobon = 0;
	while (akbob);
	DrawBobs ();
	akbob = -1;
	while (akbob);
	DrawBobs ();
	akbob = -1;

	custom.intena = INTF_BLIT;
	custom.dmacon = DMAF_RASTER | DMAF_SPRITE;

	load_colors( compcolors,-1 );

	*bpl0ptr = 0xC200;
	*hiptr = 0x3C;
	*(hiptr+2) = 0;
	*(hiptr+4) = 0;

	for( i = 0; i < MAXPLANES; i ++ ) {
		j = BitPlaneA[i] - 2;
		*bpptr[i]     = j >> 16;
		*(bpptr[i]+2) = j & 0xFFFF;
	}
	*delayptr = 0;

	ptr = (UWORD *)BitPlaneA[0];
	for( j = 0; j < MAXPLANES*WIDTHS*HEIGHT/16; j ++ ) *ptr ++ = 0;
	make_computer ();

	custom.dmacon = DMAF_SETCLR | DMAF_RASTER;
	ccount = 0;
	swap_marmor ();

	do_sound (15);
	custom.aud [0].ac_vol = 10;

	put_line( "A 9000" );
	put_line( "PREPARING CONTACT" );
	put_points ();
	for( j = 0; j < 70000; j ++ );
	if( infected[level] & (1 << no) ) {
		for( j = 0; j < 100000; j ++ );
		put_line( "SYSTEM FAILURE !" );
		for( j = 0; j < 100000; j ++ );
		put_line( "ENDCLI" );
		put_line( "+" );
	} else {
		if (message == -1) {
			do_sound (16);
			put_line ("BUILT UP CONTACT.");
			for( j = 0; j < 40000; j ++ );
			put_line ("UNKNOWN SYSTEM !");
			for( j = 0; j < 40000; j ++ );
			put_line ("SCANNING ALIEN SYSTEM.");
			put_points ();
			put_line ("SYMBOL MEMORY NOW AVAILABLE.");
			for( j = 0; j < 40000; j ++ );
			put_line ("TRANSLATION RUNNING.");
			put_points ();
			put_line ("FOREIGN NAMES AND TECHNICAL TERMS");
			put_line ("WILL BE REPLACED BY ARTIFICIAL");
			put_line ("GERMAN WORDS.");
			kill_chan_with_pri (13);
			message = 0;
		}
		put_line( "SYSTEM READY." );
		put_line( "+" );
		if (pcomno [level] == no) procom = 1;
		else procom = 0;
		for (;;) {
			if ( get_words () ) {
				put_line ("TO MANY ARGUMENTS+");
				continue;
			}
			if (handle_system ()) break;
		}
	}

	do_sound (19);
	put_line( "ACCESS TERMINATED" );
	put_line( "+" );

	for( j = 0; j < 100000; j ++ );
	ccount = -1;
	swap_marmor ();

	custom.dmacon = DMAF_RASTER;

	load_colors( colortable,2 );
	myxd = 0;
	ScrollBG ();
	load_colors( colortable,dark );

	*bpl0ptr = 0x5200;
	*hiptr = 0x38;
	*(hiptr+2) = 0x28;
	*(hiptr+4) = 0x28;

	make_display ();

	custom.dmacon = DMAF_SETCLR | DMAF_RASTER | DMAF_SPRITE;
	custom.intena = INTF_SETCLR | INTF_BLIT;
	custom.aud [0].ac_vol = 20;
	bobon = 1;
}

get_words ()
{
register int i, j;

		line = get_line();
		for( i = 0;line[i] != 0x20 && line[i] != 0; i ++ ) word1[i] = line[i];
		word1[i] = 0;
		while( line[i] == 0x20 ) i ++;
		j = i;
		for( ;line[i] != 0x20 && line[i] != 0; i ++ ) word2[i-j] = line[i];
		word2[i-j] = 0;
		while( line[i] == 0x20 ) i ++;
		j = i;
		for( ;line[i] != 0x20 && line[i] != 0; i ++ ) word3[i-j] = line[i];
		word3[i-j] = 0;
		while( line[i] == 0x20 ) i ++;
		if( line[i] != 0x00 ) {
			return 1;
			enigma1 ();
		}
		if( i - j >= 2 && word3 [0] >= '0' && word3 [0] <= '9' && word3 [1] >= '0' && word3 [1] <= '9' ) {
			zahl = (word3 [0] - '0') * 10 + (word3 [1] - '0');
		} else if( i - j >= 1 && word3 [0] >= '0' && word3 [0] <= '9' ) {
			zahl = (word3 [0] - '0');
		} else zahl = -1;

return 0;
}


do_air ( lev )
register int lev;
{
register int i;

	clr_comp ();
	put_line( "AIR CONDITIONING PROGRAMM" );
	put_line( "BY ahecbfd" );
	put_line( "AWAITING COMMANDS" );
	put_line( "USE SPACE WHEN FINISHED" );
	put_line( "" );
	for( i = 0; i < 5; i ++ ) ing[i] = 0;
	put_line( "ELEMENTS  AMOUNT" );
	for( i = 0; i < 5; i ++ ) {
		airparts[i][10] = ing[i] + 0x30;
		put_line( airparts[i] );
	}
	do {

		i = fkey ();

		if( i >= 0 && i < 5 && ing[0]+ing[1]+ing[2]+ing[3]+ing[4] < 10) {
			if( ing[i] < 9 ) ing[i] ++;
			put_char( ascii( 0x30 + ing[i] ), 10, i + 7 );
		}
		else if( i >= 5 && i < 10 && ing[0]+ing[1]+ing[2]+ing[3]+ing[4] > 0) {
			i -= 5;
			if( ing[i] > 0 ) ing[i] --;
			put_char( ascii( 0x30 + ing[i] ), 10, i + 7 );
		}
	} while( i != 10 );

	if( ing[0] + ing[1] + ing[2] + ing[3] + ing[4] != 10 ) {
		put_line( "" );
		put_line( "PARAMETER ERROR  !" );
		put_line( "ATMOSPHERE RESET !" );
		ing[0] = ing[1] = ing[2] = 0;
		ing[3] = 7;
		ing[4] = 3;
	}

	for( i = 0; i < 5; i ++ ) air_level[lev][i] = ing[i];
	if( ing[1] + ing[2] == 10 && ing[2] >= 2 && ing[2] <= 3) air_bad &= 0xFF - (1 << lev);
	else air_bad |= (1 << lev);
	green = (air_bad >> level) & 1;

	put_line( "" );
	put_line( "TASK FINISHED+" );
	put_line( "CLI READY+" );
	put_line( "+" );
}

moebius ()
{
	do_sound ( 0 );
	infected[level] |= 1 << no;
	put_line( "MOEBIUS IS WORKING !" );
	put_line( "+" );
	do_sound ( 0 );
}
