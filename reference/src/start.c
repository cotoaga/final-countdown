#define COUNTPLANES 5L
#define OFFSET 1200
extern struct ExecBase *SysBase;
extern struct IntuitionBase *IntuitionBase;

struct Interrupt VertBIntr;
WORD go_on = 0, dbuf;
void VertServer ();

LONG lines[50];

UWORD colortable [32];
UWORD colortable2 [32] = {
    0x0,0xFFF,0xFFF,0xFFF,0xFFF,0xFFF,0xFFF,0xFFF,
    0xEEF,0xDDF,0xCCF,0xDDF,0xCCF,0xAAF,0x99E,0xAAF,
    0x78D,0x328,0x55A,0xFFF,0x333,0xFFF,0x888,0xFFF,
    0xFFF,0xFFF,0xFFF,0xFFF,0xFFF,0xFFF,0x000,0x256
};

UWORD loosercol [32] = {
	0x000,0x900,0xF00,0xC00,
	0x900,0x600,0x004,0x118,
	0x229,0x23B,0x55A,0x67D,
	0x9AE,0xABF,0x430,0x530,
	0x641,0x742,0x843,0x954,
	0x111,0x222,0x333,0x444,
	0x666,0x777,0x888,0x999,
	0xBBB,0xDDD,0x300,0x400
};

UWORD compcolors [16] = {
	0x0,0x910,0x5,0x0,0x3,0x112,0x223,0x334,
	0x445,0x556,0x667,0x889,0xAAB,0xCCD,0xF00,0xC0
};

UWORD titlecolors [] = {
	0x000,0x9AC,0x89B,0x78A,
	0x679,0x568,0x457,0x346,
	0x474,0x585,0x595,0x6A6,
	0x6B7,0x7C8,0x8D9,0x9EB,
	0x89B,0x346,0x235,0x000,
	0x000,0x000,0x000,0x000,
	0x000,0xBCE,0xCDF,0xDEF,
	0xEFF,0x235,0x013,0x364
};
UWORD titlecolors2[] = {
	0x000,0x0,0x0,0x0,
	0x0  ,0x0,0x0,0x0,
	0xAA9,0x998,0x776,0x554,
	0x443,0x332,0x221,0x110,
	0x000,0x0,0x0,0xBDF,
	0x8CE,0x4AE,0x19E,0x17D,
	0x05C,0x039,0x008,0x006,
	0x004,0x0,0x0,0x0
};

UWORD syscolors [16] = {
	0x0,0x910,0x0,0x0,0x3,0x112,0x223,0x334,
	0x445,0x556,0x667,0x889,0xAAB,0xCCD,0xD00,0xA0
};

struct DiskFiles {
	char *file [8];
} diskfiles [8] = {
	{	" DESOLATOR.TXT",
		" DERURTACHYN.PRG",
		" HELP.TXT",
		" DESCRIPTION.PIC",
		" ",
		" ",
		" ",
		" " },
	{	" ASSIMILATOR.TXT",
		" DERURASSIM.PRG",
		" HANDLING.TXT",
		" TECHNIK.PIC",
		" ",
		" ",
		" ",
		" " },
	{	" VICHOROFF.TXT",
		" DERURVIHF.PRG",
		" DOCUMENT.TXT",
		" TECHNIK.PIC",
		" TARYN.TXT",
		" DERURTRN.PRG",
		" HELP.TXT",
		" DESCRIPTION.PIC" },
	{	" DERUR.PRG",
		" MOEBIUS.PRG",
		" HELP.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" NASKUR.TXT",
		" DERURNSKR.PRG",
		" HANDLING.TXT",
		" TECHNIK.PIC",
		" ",
		" ",
		" ",
		" " },
	{	" DEIMARTRANS.TXT",
		" CORURDEIMAR.PRG",
		" DOCUMENT.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" ORDER298.PRG",
		" ORDER096.PRG",
		" DESCRIPT.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" CLIMATE.PRG",
		" CODES.TXT",
		" USAGE.TXT",
		" ",
		" ",
		" ",
		" ",
		" " }
};

struct ExitDef {
	int x, y, a;
} exitpos [6];
struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS];
struct CompDef {
	int x, y, typ, files, accessmode [MAXDEVS];
} comp [MAXCOMPS];
struct MyFiles {
	char file [8][20];
} compfiles [MAXCOMPS];
struct FoeDef {
	long txl, txr;
	int y, v, img, mode;
} foedef [MAXFOES];
struct StartDef {
	int x [2], y [2];
} startdef [MAXLEVELS] = {
	{ 45*16, 80*16, 4*16*64+704, 4*16*64+704 },
	{ 54*16, 25*16, 8*16*64+704, 8*16*64+704 },
	{  0*16,  0*16, 0*16*64+704, 4*16*64+704 },
	{ 99*16, 70*16, 0*16*64+704, 0*16*64+704 },
	{ 66*16, 9*16, 4*16*64+704, 0*16*64+704 },
	{ 61*16, 61*16, 0*16*64+704, 0*16*64+704 },
	{  0*16, 80*16, 4*16*64+704, 4*16*64+704 },
	{ 63*16, 63*16, 4*16*64+704, 4*16*64+704 }
};

extern UWORD lomem;
extern WORD cyccol [8][4][4];

struct GfxBase *GfxBase;
struct FileHandler *fh, *Open ();
extern BYTE *OpenLibrary ();
UWORD *AllocMem ();
LONG AvailMem ();

UWORD *bpptr [MAXPLANES], *delayptr, *wptr, *sprptr, *tiptr;
UWORD *txstart, *tystart, *txstop, *tystop, *bmod, *skip;
UWORD matrix [15][128];
UWORD *ground [350], *clears;
ULONG Background[2], logo, asteroid, ttext;
UWORD xpos;
ULONG spos;

LONG BitPlaneA [MAXPLANES], BitPlane2[MAXPLANES], BobPlane [MAXPLANES];
BYTE *minesptr [MAXPLANES], *scoreptr [MAXPLANES], *barptr [MAXPLANES][3];

UWORD *spritechannel [2][8], *vspritei [16], *foemem [8][8];
UWORD vspriteon [16], vspritex [16], vspritey [16], vspritedma [16], sprites;

UWORD *girlgr [16], *girlgl [16];
UWORD *girlar [4], *girlal [4];
UWORD *girlgsr [8], *girlgsl [8];
UWORD *girlasr [8], *girlasl [8];
UWORD *girld [4], *girle [21], *girltr [8];

UWORD *marptr, *objmem [4], *font, *computer;
BYTE *tepo, *textptr, chars [10][64];

UWORD *scopper1, *copperlist, *clist, *colptr, *colorptr, *tcolorptr, *colptr2;
UWORD *bpl0ptr, *hiptr;

LONG textlen = 3000, gtime, score;
WORD collision, fuel, life, level, master, green;
WORD sintable [360];
LONG titleptr;

extern WORD button, devs, comps, foes, dark;

struct Sound {
	long start, length;
	int speed, vol, pri;
} sound [] = {
	0,2974, 428,64, 7,
	0,6940, 856,64, 7,
	0,2544, 856,64, 7,
	0,1380,1072,64,15,
	0,7536, 856,20,11,
	0,7972,1440,64, 6,
	0,4194, 856,64, 9,
	0,4138,1712,64, 5,
	0,4462,1712,64, 5,
	0,2964, 760,64, 5,
	0,6008,1712,64, 3,
	0,9102,1000,64, 8,
	0,4500, 428,64,12,
	0,4124,1712,64, 6,
	0,4416, 856,64, 6,
	0,4438,1712,64, 9,
	0,7470, 856,64,13,
	0,4296, 856,64,14,
	0, 474, 856,64, 8,
	0,5462, 856,64, 9,
	0,1156, 680,32, 4,
};

struct Sound syssound [] = {
	0,9634, 428,64, 7,
	0,9094, 428,64, 7,
	0,3086, 428,64,13,
	0,11310,904,20,12,
};

UWORD *mt_data;

load_music ()
{

	fh = Open ("DISK00:Songs/Title", MODE_OLDFILE);
	mt_data = AllocMem (88490L, MEMF_CHIP | MEMF_CLEAR);
	Read (fh, mt_data, 88490L);
	Close (fh);
;
#asm
	movem.l d0-d7/a0-a6,-(sp)
	jsr _geta4#

	jsr mt_init
	public mt_init

	movem.l (sp)+,d0-d7/a0-a6
	moveq.l	#1,D0
#endasm
;
}

free_music ()
{

	custom.dmacon = DMAF_AUDIO;
#asm
	movem.l d2/d3/a4,-(sp)
	jsr _geta4#

	jsr mt_end
	public mt_end

	movem.l (sp)+,d2/d3/a4
	moveq.l	#1,D0
#endasm

	FreeMem (mt_data, 88490L);
}

load_credits ()
{
register LONG *ptrD, *ptrS;
register int i;
	fh = Open ("DISK00:GFX/Count", MODE_OLDFILE);

	Background[0]  = (long)AllocMem (71680L, MEMF_CHIP);
	Background[1]  = (long)AllocMem (71680L, MEMF_CHIP);

	Read (fh, Background[0], 71680L);
	ptrS = (LONG *)Background [0];
	ptrD = (LONG *)Background [1];
	for( i = 0; i < (int)(71680/4); i ++ ) *ptrD ++ = *ptrS ++;

	Close (fh);

	fh = Open ("DISK00:GFX/Logo", MODE_OLDFILE);
	logo  = (long)AllocMem (71680L, MEMF_CHIP);
	Read (fh, logo, 71680L);
	Close (fh);
}

load_asteroid ()
{
	fh = Open ("DISK00:GFX/Asteroid", MODE_OLDFILE);

	asteroid  = (long)AllocMem (9122L, MEMF_CHIP);
	Read (fh, asteroid, 9122L);
	Close (fh);

	fh = Open ("DISK00:GFX/TText", MODE_OLDFILE);
	ttext  = (long)AllocMem (7212L, MEMF_CHIP);
	Read (fh, ttext, 7212L);
	Close (fh);

}

read_text (fptr)
char *fptr;
{
register int i;

	off_blit ();
	fh = Open (fptr, MODE_OLDFILE);
	i = Read (fh, textptr, textlen-2L);
	textptr[i] = 0x0A;
	textptr[i+1] = 0x7F;
	Close (fh);
	on_blit ();
}

put_text ()
{
register int i, j;

	do_sound (16);
	for (i = 0; i < 3000; i++) if (textptr [i] == 0x0A) textptr [i] = 0;
	for (i = j = 0; textptr [i] != 0x7F; i++) {
		put_line (textptr + i);
		while (textptr [i]) ++i;
		if (++j == 11) {
			kill_chan_with_pri (13);
			key ();
			do_sound (16);
			j = 0;
		}
	}
	kill_chan_with_pri (13);
}

put_start_text ()
{
register int i, j, k;
long l;

	j = 0;
	for (i = 0; i < 3000; i++) if (textptr [i] == 0x0A) textptr [i] = 0;
	do_sound (-3);
	for (i = 0; textptr [i] != 0x7F; i++) {
		put_start_line (textptr + i);
		while (textptr [i]) ++i;
		if (++j == 11) {
			kill_chan_with_pri (13);
			for (l = 0; l < 80000 && !button; l++) joystick ();
			do_sound (-3);
			j = 0;
		}
		joystick ();
		if (!button) for (l = 0; l < 15678; l++);
	}
	kill_chan_with_pri (13);
}

init_sprite ()
{
register int i, j;

	for( i = 0; i < 8; i ++ ) for( j = 0; j < 2; j ++ ) 
		spritechannel[j][i] = AllocMem( 1500L, MEMF_CHIP | MEMF_CLEAR);
}

load_sounds ()
{
register int i;

	if( lomem ) {
		return -1;
		enigma7 ();
	}
	off_blit ();
	fh = Open ("DISK00:SND/Sound", MODE_OLDFILE);
	for (i = 0; i < 21; i++) {
		sound [i].start = (long) AllocMem (sound [i].length+2, MEMF_CHIP | MEMF_CLEAR);
		Read (fh, sound [i].start+2, sound [i].length);
	}
	Close (fh);
	on_blit ();
}

free_sounds ()
{
register int i;

	if( lomem ) return;
	for (i = 0; i < 21; i++) {
		FreeMem (sound [i].start, sound [i].length+2);
	}
}

load_syssounds ()
{
register int i;

	if( lomem ) return -1;
	off_blit ();
	fh = Open ("DISK00:SND/SSound", MODE_OLDFILE);
	for (i = 0; i < 4; i++) {
		syssound [i].start = (long) AllocMem (syssound [i].length+2, MEMF_CHIP | MEMF_CLEAR);
		Read (fh, syssound [i].start+2, syssound [i].length);
	}
	Close (fh);
	on_blit ();
}

free_syssounds ()
{
register int i;

	if( lomem ) return;
	for (i = 0; i < 4; i++) {
		FreeMem (syssound [i].start, syssound [i].length+2);
	}
}

load_stuff ()
{
	fh = Open ("DISK00:gfx/Stuff", MODE_OLDFILE);
	load_sintable ();
	load_obj ();
	load_bobs ();
	load_font ();
	load_chars ();
	load_computer ();
	load_marmor ();
	Close (fh);
}

load_computer ()
{
	computer = (UWORD *) AllocMem (10188L, NULL);
	Read (fh, computer, 10188L);
}

load_syscomp ()
{
	off_blit ();
	fh = Open ("DISK01:gfx/SysCompC", MODE_OLDFILE);

	gdecom( BitPlaneA [0], WIDTHS*HEIGHT/8, WIDTHS/8 );

	Close (fh);
	on_blit ();
}

load_sintable ()
{
	Read (fh, sintable, 720L);
}

load_obj ()
{
register int i, j;
register UWORD *ptr;

	for( i = 0; i < 4; i ++ ) {
		objmem[i] = (UWORD *)AllocMem( 194L, MEMF_CLEAR );
		Read (fh, objmem [i] , 194L);
		ptr = objmem[i] + 81;
		for( j = 0; j < 16; j ++ ) *ptr++ ^= 0xFFFF;
	}
}

load_font ()
{
	font = AllocMem( 1920L, MEMF_CLEAR );
	Read (fh, font, 1920L);
}

load_chars ()
{
register int i, ii;

	for (i = 0; i < MAXPLANES; i++) {
		scoreptr [i] = (char *) BitPlane2 [i]+4*WIDTH/8+12;
		minesptr [i] = (char *) BitPlane2 [i]+4*WIDTH/8+25;
		for (ii = 0; ii < 3; ii++) barptr [i][ii] = (char *) BitPlane2 [i]+((ii+15)*WIDTH/8)+4;
	}

	for (i = 0; i < 10; i++) Read (fh, chars [i], 40L);
}

load_marmor ()
{
register int i;

	Read (fh, BitPlane2[0], 4180L);
	marptr = AllocMem (4180L, NULL);
	Read (fh, marptr, 4180L);
}

load_struct (level)
int level;
{
int i, j;
char name [20];

	strcpy (name, "DISK01:lev/Struct0");
	name [17] += level;

	fh = Open (name, MODE_OLDFILE);
	Read (fh, exitpos, (long) (sizeof (struct ExitDef) * 6));
	Read (fh, comp, (long) (sizeof (struct CompDef) * MAXCOMPS));
	Read (fh, device, (long) (sizeof (struct DevDef) * MAXDEVS));

	for (i = 0; i < MAXCOMPS; i++)
		for (j = 0; j < 8; j++)
			Read (fh, compfiles [i].file [j], 20L);

	Read (fh, foedef, (long) (sizeof (struct FoeDef) * MAXFOES));
	Read (fh, colortable, 64L);
	Close (fh);

	foes = 0; while (foedef [foes].txl) ++foes;
	devs = 0; while (device [devs].x) ++devs;
	comps = 0; while (comp [comps].x) ++comps;
}

load_bobs ()
{
register int i;
char w, h, *ptr;

	for (i = 0; i < 16; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girlgr [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girlgr [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 16; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girlgl [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girlgl [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 4; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girld [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girld [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 21; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girle [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girle [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 4; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girlar [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girlar [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 4; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girlal [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girlal [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 8; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girlasl [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girlasl [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 8; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girlasr [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girlasr [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 8; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girltr [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girltr [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 8; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girlgsr [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girlgsr [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
	for (i = 0; i < 8; i++) {
		Read (fh, &w, 1L);
		Read (fh, &h, 1L);
		girlgsl [i] = AllocMem ((long) (2+w*h*12), MEMF_CHIP );
		ptr = (char *) girlgsl [i];
		ptr [0] = w; ptr [1] = h;
		Read (fh, ptr+2, (long) (w*2*h*6));
	}
}

load_matrix (level)
int level;
{
register int x;
char name [23];

	strcpy (name, "DISK01:LEV/Matrix0");
	name [17] += level;

	if( (fh = Open (name, MODE_OLDFILE)) > 0 ) {
		Read (fh, matrix, 3072L);
		Close (fh);
	}
	for( x = 0; x < 128; x ++ ) matrix[12][x] = matrix[13][x] = matrix[14][x] = 0;
}

load_graphics (level)
int level;
{
int no, h;
register int i, n, sec;
char name1 [23], name2 [23];
static int oldsec = -1;

	strcpy (name1, "DISK01:lev/Elements0");
	strcpy (name2, "DISK01:LEV/Foes0");
	if (level == 7) {
		name1 [19] = '3';
		name2 [15] = '3';
		sec = 3;
	} else if (level >= 4 && level <= 6) {
		name1 [19] = '2';
		name2 [15] = '2';
		sec = 2;
	} else if (level == 1 || level == 2) {
		name1 [19] = '1';
		name2 [15] = '1';
		sec = 1;
	} else  {
		name1 [19] = '0';
		name2 [15] = '0';
		sec = 0;
	}

	if (sec == oldsec) return;
	else oldsec = sec;

	fh = Open (name1, MODE_OLDFILE);
	Read (fh, &no, 2L);
	Read (fh, ground [0], 54880L);
	Close (fh);

	fh = Open (name2, MODE_OLDFILE);
	for (n = 0; n < 8; n++) {
		for (i = 0; i < 8; i++) {
			Read (fh, &h, 2L);
			if (foemem [n][i]) FreeMem (foemem [n][i], (long) ((*foemem [n][i])*8+2));
			foemem [n][i] = (UWORD *)AllocMem( (long) (8*h+2), MEMF_CLEAR | MEMF_CHIP);
			Read (fh, ((char *) foemem [n][i])+2, (long) (h*8));
			*foemem [n][i] = h;
		}
	}
	Close (fh);
}

swap_marmor ()
{
register int i;
register char *ptr1, *ptr2, var;

	ptr1 = (char *) BitPlane2[0];
	ptr2 = (char *) marptr;
	for( i = 0; i < 4180; i ++ ) {
		var = *ptr1;
		*(ptr1++) = *ptr2;
		*(ptr2++) = var;
	}
}

marmor_off ()
{
register int i;

	for (i = 0; i < 32; i++)
		*(colptr2-(31*2)+(2*i)) = 0;
}

marmor_on ()
{
register int i;

	for (i = 0; i < 32; i++)
		*(colptr2-(31*2)+(2*i)) = colortable2 [i];
}

flash ()
{
register int l, i;
int c, d;

	for (l = 0; l < 20; l++) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			d = ((c >> 8) & 15) + ((c >> 4) & 15) + (c & 15);
			if( d > 14 ) *(colorptr+i*2) = (l & 1) ? 0x0F0 : 0;
			else *(colorptr+i*2) = (l & 1) ? 0 : 0x00F;
		}
		for (c = 0; c < 10000; c++);
	}
	set_colors ();
}

flash_end ()
{
register int l, i;
int c, d;

	for (l = 0; l < 16; l++) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			d = ((c >> 8) & 15) + ((c >> 4) & 15) + (c & 15);
			if( d > 14 ) *(colorptr+i*2) = (l & 1) ? 0xFFF : 0;
			else *(colorptr+i*2) = (l & 1) ? 0 : 0xFFF;
		}
		for (c = 0; c < 10000; c++);
	}
	set_colors ();
}

show_green ()
{
register int i;
register int c, d;

		for (i = 0; i < 32; i++) {
			c = colortable [i];
			d = ((c >> 4) & 15) + 4;
			if( d > 15 ) d = 15;
			*(colorptr+i*2) = (colortable[i] & 0xF0F) + (d << 4);
		}
}

color_dark (dark)
int dark;
{
register int l, i;
int c, cr, cg, cb;

	if (dark) l = 5;
	else l = 0;
	for (; l < 16; l++) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			if( green ) cg += 4;
			if( cg > 15 ) cg = 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
		for (c = 0; c < 10000; c++);
	}
}

set_colors ()
{
register int i;
int c, cr, cg, cb;

	if( dark ) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			if( green ) cg += 4;
			if( cg > 15 ) cg = 15;
			cb = c & 15;
			cr -= 5; cg -= 5; cb -= 5;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
	}	else for (i = 0; i < 32; i++) *(colorptr+i*2) = colortable[i];
}

light_off ()
{
register int l, i;
int c, cr, cg, cb;

	dark = 1;
	for (l = 0; l < 5; l++) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			if( green ) cg += 4;
			if( cg > 15 ) cg = 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
		for (c = 0; c < 10000; c++);
	}
}

light_puls ()
{
register int i, j;
int c, cr, cg, cb;

	for (j = 0; j < 16; j++) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			if (green) cg += 4;
			cr += j; cg += j; cb += j;
			if (cr > 15) cr = 15;
			if (cg > 15) cg = 15;
			if (cb > 15) cb = 15;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
		for (c = 0; c < 2000; c++);
	}
	for (; j >= 0; j--) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			if (green) cg += 4;
			cr += j; cg += j; cb += j;
			if (cr > 15) cr = 15;
			if (cg > 15) cg = 15;
			if (cb > 15) cb = 15;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
		for (c = 0; c < 10000; c++);
	}
}

color_light (dark)
int dark;
{
register int l, i;
int c, cr, cg, cb;

	for (l = 15; l >= ((dark)? 4: 0); l--) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			if( green ) cg += 4;
			if( cg > 15 ) cg = 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
		for (c = 0; c < 10000; c++);
	}
}

syscomp_light ( l )
register int l;
{
register int i;
int c, cr, cg, cb;

	for (; l >= 0; l--) {
		for (i = 0; i < 16; i++) {
			c = syscolors [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
		for (c = 0; c < 10000; c++);
	}
}

syscomp_dark ()
{
register int l, i;
int c, cr, cg, cb;

	for (l = 0; l < 16; l++) {
		for (i = 0; i < 16; i++) {
			c = syscolors [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
		for (c = 0; c < 10000; c++);
	}
}

open_view ()
{
long ptr1, ptr2, ptr3;
register int i, r, s;
register UWORD *ptr;
register long c;

	clist = copperlist = AllocMem (500L, MEMF_CHIP);

	clears = AllocMem (10L, MEMF_CHIP | MEMF_CLEAR);

	ptr1 = (long) AllocMem (MAXPLANES*WIDTHS*HEIGHT/8, MEMF_CHIP | MEMF_CLEAR);
	ptr3 = (long) AllocMem (MAXPLANES*1000L, MEMF_CHIP | MEMF_CLEAR);

	ptr2 = (long) AllocMem (MAXPLANES*WIDTH*IHEIGHT/8, MEMF_CHIP | MEMF_CLEAR);

	for (i = 0; i < MAXPLANES; i++) BitPlaneA[i]  = ptr1 + i*(WIDTHS*HEIGHT/8);
	for (i = 0; i < MAXPLANES; i++) BobPlane[i]  = ptr3 + i*1000L;

	for (i = 0; i < MAXPLANES; i++) BitPlane2[i] = ptr2 + i*(WIDTH*IHEIGHT/8);

	textptr = (BYTE *) AllocMem (textlen, NULL);
	ground [0] = AllocMem (55000L, MEMF_CHIP);
	for (i = 1; i < 350; i++) ground [i] = (UWORD *) (((long) ground [0]) + (long) (160L * (long) i));

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

	colptr2 = copperlist;
	colptr2 += 2*31+1;
	for( i = 0; i < (1 << MAXPLANES); i ++ ) {
		MOVE( 0x180+2*i, 0 );
	}
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
	hiptr = copperlist + 1;
	MOVE( 0x092, 0x0038 ); /* DDFSTART */
	MOVE( 0x108, 0x0028 ); /* BPL1MOD  */
	MOVE( 0x10A, 0x0028 ); /* BPL2MOD  */
	colorptr = colptr = (copperlist + 1);
	colptr += 2*11;
	for( i = 0; i < (1 << MAXPLANES); i ++ ) {
		MOVE( 0x180+2*i, 0 );
	}

	for( i = 0; i < MAXPLANES; i ++ ) {
		bpptr[i] = copperlist + 1;
		MOVE( 0xe0 + i*4, (BitPlaneA[i]-2) >> 16);
		MOVE( 0xe2 + i*4, (BitPlaneA[i]-2) & 0xffff);
	}

	WAIT( 0x03F, 0x000 );
	bpl0ptr = copperlist + 1;
	MOVE( 0x100, (MAXPLANES << 12) | 0x200 ); /* BPLCON0 */

	wptr = copperlist;
	WAITB( 0x0FD, 0x000 );
	MOVE( 0x09C, 0x8040 );
	WAIT( 0x0FF, 0x000 );
	MOVE( 0x180, 0x000 );
	MOVE( 0x100, 0x0200 ); /* BPLCON0 */
	WAIT( 0xFF, 0xFE   );
}

load_colors (color, dark)
UWORD *color;
int dark;
{
register int i;
int c, cr, cg, cb;

	if (!dark || dark == -1) {
		if( !green || dark == -1 ) for( i = 0; i < 32; i++) *(colorptr+i*2) = *color++;
		else show_green ();
	} else if (dark == 1) {
		for (i = 0; i < 32; i++) {
			c = *color++;
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			if( green ) cg += 4;
			if( cg > 15 ) cg = 15;
			cb = c & 15;
			cr -= 5; cg -= 5; cb -= 5;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
	} else for( i = 0; i < 32; i++) *(colorptr+i*2) = 0;
}

load_riss ( name )
char *name;
{
register int i, y;
register char *ptr;

	off_blit ();
	fh = Open (name, MODE_OLDFILE);

	gdecom( BitPlaneA [0] + 9*80 + 12, WIDTHS*HEIGHT/8, WIDTHS/8 );

	Close (fh);
	on_blit ();
}

load_pic ( name )
char *name;
{
register int i, y;
register char *ptr;

	off_blit ();
	fh = Open (name, MODE_OLDFILE);
	for (i = 0; i < MAXPLANES; i++) {
		ptr = (char *)BitPlaneA [i];
		for( y = 0; y < 192; y ++ ) {
			Read (fh, ptr, 38L);
			ptr += 80;
		}
	}
	Close (fh);
	on_blit ();
}

clr_comp ()
{
int i;
register int y, x;
register WORD *ptr, fill;

	for (i = 0; i < MAXPLANES-1; i++) {
		if (i == 1) fill = 0xFFFF;
		else fill = 0;
		ptr = (WORD *) (BitPlaneA [i] + 9*80 + 12);
		for( y = 0; y < 96; y ++ ) {
			for (x = 0; x < 20; x++) *(ptr++) = fill;
			ptr += 20;
		}
	}
}

no_plot()
{
register int x, y;

	for( y = -12; y <= 12; y ++ )
		for( x = -32; x <= 32; x ++ ) unplot( x, y );
}



title_pic ()
{
register int i, j, r, s;
long l;
register LONG *ptrS, *ptrD;

	GfxBase = (struct GfxBase *) OpenLibrary ("graphics.library", 0L);
	WaitTOF ();
	custom.dmacon = DMAF_SPRITE | DMAF_RASTER;


	load_credits ();
	load_asteroid ();

	scopper1 = copperlist = AllocMem (500L, MEMF_CHIP);

	MOVE( 0x100, 0x0200 ); /* BPLCONx */
	MOVE( 0x102, 0x0000 );
	MOVE( 0x104, 0x0024 );
	txstart = copperlist + 1;
	MOVE( 0x092, 0x0038 ); /* DDFSTART */
	txstop  = copperlist + 1;
	MOVE( 0x094, 0x00D0 ); /* DDFSTOP  */
	MOVE( 0x08E, 0x2C81 ); /* DIWSTART */
	MOVE( 0x090, 0x2CC1 ); /* DIWSTOP  */
	bmod = copperlist + 1;
	MOVE( 0x108, 0x0010 ); /* BPL1MOD  */
	MOVE( 0x10A, 0x0010 ); /* BPL2MOD  */

	tcolorptr = copperlist + 1;
	for( i = 0; i < 32; i ++ ) {
		MOVE( 0x180+2*i, 0 );
	}
	WAIT( 0x02A, 0x000 );
	tiptr = copperlist + 1;
	for( i = 0; i < COUNTPLANES; i ++ ) {
		MOVE( 0xe0 + i*4, 0);
		MOVE( 0xe2 + i*4, 0);
	}
	tystart = copperlist;
	WAIT( 0x02C, 0x000 );
	MOVE( 0x100, (COUNTPLANES << 12) | 0x0200 ); /* BPLCON0 */

	tystop  = copperlist;
	WAIT( 0x085, 0x000 );
	skip = copperlist + 1;
	MOVE( 0x100, (COUNTPLANES << 12) | 0x200 ); /* BPLCON0 */
	WAIT( 0xFF, 0xFE   );

	Disable ();
	WaitTOF ();
	custom.cop1lc  = (long)scopper1;

	custom.dmacon = DMAF_SETCLR | DMAF_RASTER;

	Enable ();

	for( r = 0; r < COUNTPLANES; r ++ ) {
		*(tiptr+4*r)   = ((logo+(71680/5L)*r) >> 16);
		*(tiptr+4*r+2) = ((logo+(71680/5L)*r) & 0xffff);
	}
	title_light ();
	button = 0;
	while (!button) joystick ();
	title_dark ();

	for( i = 63; i >= 5; i -= 4 ) {
		for( j = 0; j < 7; j -- ) {
			custom.aud [0].ac_vol = custom.aud [1].ac_vol = i;
			custom.aud [2].ac_vol = custom.aud [3].ac_vol = i;
		}
	}
	custom.aud [0].ac_vol = custom.aud [1].ac_vol = 0;
	custom.aud [2].ac_vol = custom.aud [3].ac_vol = 0;
	rem_int1 ();
	free_music ();

	ptrD = (LONG *) logo;
	ptrS = (LONG *) Background[0];
	for( i = 0; i < (int)(71680/4); i ++ ) *ptrD++ = *ptrS ++;
	for( r = 0; r < COUNTPLANES; r ++ ) {
		*(tiptr+4*r)   = ((Background[0]+(71680/5L)*r) >> 16);
		*(tiptr+4*r+2) = ((Background[0]+(71680/5L)*r) & 0xffff);
	}
	dbuf = 1;
	title_light_f ();
	button = 0;
	i = 0;
	Forbid ();
	init_si ();

	OwnBlitter ();
	custom.dmacon = 0x8400;
	i = -255;
	for( l = 0; l < 2700 && !button; l ++ ) {
		go_on = 0;
		while ( !go_on );
		if( l == 150 || l == 151 ) blitter( ttext, 0, 200 );
		if( l == 350 || l == 351 ) blitter_clr( ttext, 0, 200 );

		if( l == 450 || l == 451 ) blitter( ttext+2*1202, 0, 200 );
		if( l == 650 || l == 651 ) blitter_clr( ttext+2*1202, 0, 200 );

		if( l == 750 || l == 751 ) blitter( ttext+1202, 0, 200 );
		if( l == 950 || l == 951 ) blitter_clr( ttext+1202, 0, 200 );

		if( l == 1400 || l == 1401 ) blitter( ttext+3*1202, 0, 200 );
		if( l == 1600 || l == 1601 ) blitter_clr( ttext+3*1202, 0, 200 );

		if( l > OFFSET && l <= OFFSET + 896 ) {
			i ++;
			blitter ( asteroid, i / 2, 80 );
			if( l >= OFFSET + 400 ) {
				*(tcolorptr+ 5*2) = cyccol [0][(l>>3)&3][0];
				*(tcolorptr+ 6*2) = cyccol [0][(l>>3)&3][1];
				*(tcolorptr+ 7*2) = cyccol [0][(l>>3)&3][2];
				*(tcolorptr+17*2) = cyccol [0][(l>>3)&3][3];
			}
			if( l >= OFFSET + 450 ) {
				*(tcolorptr+ 1*2) = cyccol [0][(l>>4)&3][0];
				*(tcolorptr+ 2*2) = cyccol [0][(l>>4)&3][1];
				*(tcolorptr+ 3*2) = cyccol [0][(l>>4)&3][2];
				*(tcolorptr+ 4*2) = cyccol [0][(l>>4)&3][3];
			}
			if( l >= OFFSET + 500 ) {
				*(tcolorptr+18*2) = cyccol [0][(l>>2)&3][0];
				*(tcolorptr+29*2) = cyccol [0][(l>>2)&3][1];
				*(tcolorptr+30*2) = cyccol [0][(l>>2)&3][2];
				*(tcolorptr+31*2) = cyccol [0][(l>>2)&3][3];
			}
		}
		joystick ();
	}
	custom.dmacon = 0x0400;
	DisownBlitter ();
	RemIntServer( INTB_VERTB, &VertBIntr );
	Permit ();

	title_dark_f ();

	*txstart = 0x38;
	*txstop  = 0xD0;
	*tystart = (0x2C << 8) | 1;
	*tystop  = (0xFF << 8) | 1;
}

end_titlepic ()
{

	custom.dmacon = DMAF_RASTER;
	custom.cop1lc  = (long)clist;
	FreeMem (scopper1, 500L);
	FreeMem (ttext, 7212L);

}

free_credits ()
{

	FreeMem (Background[0], 71680L);
	FreeMem (Background[1], 71680L);
	FreeMem (logo,    71680L);
	FreeMem (asteroid, 8930L);

}

title_light ()
{
register int i, l;
int c, cr, cg, cb;

	for (l = 15; l >= 0; l--) {
		for (i = 0; i < 32; i++) {
			c = titlecolors [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(tcolorptr+i*2) = c;
		}
		for (c = 0; c < 6000; c++);
	}
}

title_dark ()
{
register int l, i;
int c, cr, cg, cb;

	for (l = 0; l < 16; l++) {
		for (i = 0; i < 32; i++) {
			c = titlecolors [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(tcolorptr+i*2) = c;
		}
		for (c = 0; c < 6000; c++);
	}
}


title_light_f ()
{
register int i, l;
int c, cr, cg, cb;

	for (l = 15; l >= 0; l--) {
		for (i = 0; i < 32; i++) {
			c = titlecolors2 [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(tcolorptr+i*2) = c;
		}
		for (c = 0; c < 6000; c++);
	}
}

title_dark_f ()
{
register int l, i;
int c, cr, cg, cb;

	for (l = 0; l < 16; l++) {
		for (i = 0; i < 32; i++) {
			c = titlecolors2 [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			cr -= l; cg -= l; cb -= l;
			if (cr < 0) cr = 0;
			if (cg < 0) cg = 0;
			if (cb < 0) cb = 0;
			c = cb | (cg << 4) | (cr << 8);
			*(tcolorptr+i*2) = c;
		}
		for (c = 0; c < 6000; c++);
	}
}

disk_change ()
{
register int r;
register long q;

	*bmod    = 0;
	*(bmod+2)= 0;
	*txstart = 0x38;
	*txstop  = 0xD0;
	*tystart = (0x80 << 8) | 1;
	*tystop  = (0x85 << 8) | 1;
	*skip    = 0x200;
	for( r = 0; r < 5; r ++ ) {
		*(tiptr+4*r)   = ((ttext+5*1202+200*r+2) >> 16);
		*(tiptr+4*r+2) = ((ttext+5*1202+200*r+2) & 0xffff);
	}
	title_light_f ();
	off_blit ();
	while( ( fh=Open("DISK01:gfx/SysCompC",MODE_OLDFILE) ) < 1 );
	Close (fh);
	on_blit ();
	title_dark_f ();
	CloseWorkBench ();
	for( r = 0; r < 5; r ++ ) {
		*(tiptr+4*r)   = ((ttext+4*1202+200*r+2) >> 16);
		*(tiptr+4*r+2) = ((ttext+4*1202+200*r+2) & 0xffff);
	}
	title_light_f ();
	for (q = 0; q < 500000; q++);
	title_dark_f ();
}

light_end ()
{
register int i, j;
int c, cr, cg, cb;

	do_sound (1);
	for (j = 0; j < 16; j++) {
		for (i = 0; i < 32; i++) {
			c = colortable [i];
			cr = (c >> 8) & 15;
			cg = (c >> 4) & 15;
			cb = c & 15;
			if (green) cg += 4;
			cr += j; cg += j; cb += j;
			if (cr > 15) cr = 15;
			if (cg > 15) cg = 15;
			if (cb > 15) cb = 15;
			c = cb | (cg << 4) | (cr << 8);
			*(colorptr+i*2) = c;
		}
		for (c = 0; c < 10000; c++);
	}
	custom.dmacon = DMAF_RASTER;
	do_sound (11);
	do_sound (11);
	do_sound (11);
	flash_end ();
	flash_end ();
	flash_end ();
}


load_endtexte ()
{
register int i, j, k;

	off_blit ();
	fh = Open ("DISK01:Txt/TxtFFF", MODE_OLDFILE);
	i = Read (fh, textptr, textlen-2L);
	textptr[i] = 0x00;
	k = 1;
	lines[0] = (LONG)textptr;
	for( j = 0; j < i; j ++ ) {
		if( textptr[j] == 0x0A ) {
			textptr[j] = 0;
			lines[k] = (long)(textptr + j + 1);
			k ++;
			if( k >= 45 ) break;
		}
	}
	Close (fh);
	on_blit ();

}

blitter( image, x, y )
int x, y;
char *image;
{
	int addb;
	long addressD, addressS;
	static int nr = 0, ldbuf;
	register int i, wordwidth, height, h;

	wordwidth = image [0];
	height = image [1];
	image += 2;

	i = ((x >> 3) + y*(448/8)) & 0xFFFE;
	addressS = i + logo;
	addressD = i + Background[dbuf];
	addb = (wordwidth * height) << 1;

	x = (x & 15) << 12;

	custom.bltamod  = -2;
	custom.bltbmod  = -2;
	custom.bltcmod  = 448/8 - 2 - (wordwidth << 1);
	custom.bltdmod  = 448/8 - 2 - (wordwidth << 1);
	custom.bltafwm  = 0xffff;
	custom.bltalwm  = 0;
	custom.bltcon1  = x;
	custom.bltcon0  = DEST | SRCA | SRCB | SRCC | 0xca | x;
	for (i = 0; i < COUNTPLANES; i++) {
		while( (custom.dmaconr & 0x4000) );
		custom.bltapt   = (APTR) (image +  COUNTPLANES*addb);
		custom.bltbpt = (APTR) (image + i*addb);
		custom.bltcpt = (APTR) (addressS + (71680/5L)*i);
		custom.bltdpt = (APTR) (addressD + (71680/5L)*i);
		custom.bltsize  = (height << 6) + wordwidth + 1;
	}
	while( (custom.dmaconr & 0x4000) );

}

blitter_clr( image, x, y )
int x, y;
char *image;
{
	int addb;
	long addressD, addressS;
	static int nr = 0, ldbuf;
	register int i, wordwidth, height, h;

	wordwidth = image [0];
	height = image [1];
	image += 2;

	i = ((x / 8) + y*(448/8)) & 0xFFFE;
	addressS = i + logo;
	addressD = i + Background[dbuf];
	addb = (wordwidth * height) << 1;

	custom.bltcmod  = 448/8 - 2 - (wordwidth << 1);
	custom.bltdmod  = 448/8 - 2 - (wordwidth << 1);
	custom.bltcon1 = NULL;
	custom.bltcon0 = DEST | SRCB | 0xcc;
	for (i = 0; i <  COUNTPLANES; i++) {
		while( (custom.dmaconr & 0x4000) );
		custom.bltbpt = (APTR) (addressS + (71680/5L)*i);
		custom.bltdpt = (APTR) (addressD + (71680/5L)*i);
		custom.bltsize = wordwidth + 1 + (height << 6);
	}
}

init_si ()
{
register int i;

	VertBIntr.is_Node.ln_Type = NT_INTERRUPT;
	VertBIntr.is_Node.ln_Pri  = 0;
	VertBIntr.is_Node.ln_Name = "MouseInt";
	VertBIntr.is_Data = 0;
	VertBIntr.is_Code = VertServer;
	AddIntServer (INTB_VERTB, &VertBIntr);

}

void VertServer ()
{
int r;
#asm
	movem.l d2/d3/a4,-(sp)
	jsr _geta4#
#endasm

	for( r = 0; r < COUNTPLANES; r ++ ) {
		*(tiptr+4*r)   = ((Background[dbuf]+(71680/5L)*r) >> 16);
		*(tiptr+4*r+2) = ((Background[dbuf]+(71680/5L)*r) & 0xffff);
	}
	dbuf ^= 1;

	go_on = 1;

;
#asm
	movem.l (sp)+,d2/d3/a4
	moveq.l	#0,D0
#endasm
}
