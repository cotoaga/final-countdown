extern LONG BitPlane2 [MAXPLANES];
extern BYTE *minesptr [MAXPLANES], *scoreptr [MAXPLANES], *barptr [MAXPLANES][3];
extern LONG score, gtime;
extern WORD fueladd, fuel, life, level;
extern UWORD *colptr2;
extern BYTE chars [10][64];

WORD minesage, scoreage [MAXDIGITS];
WORD mines, scoredigit [MAXDIGITS];
WORD oldmines, oldscoredigit [MAXDIGITS];

disp_fuel ()
{
static int oldfuel = -1;
register int offs, plane, line;
register char val;
int rest;

	if (fuel >> 2 != oldfuel) {
		oldfuel = fuel >> 2;
		offs = oldfuel >> 3;
		rest = oldfuel % 8;
		if (!rest) {
			*colptr2 = (0x00f0 + (fuel >> 6)) << 4;
			for (line = 0; line < 3; line++) {
				for (plane = 0; plane < MAXPLANES; plane++)
					*(barptr [plane][line] + offs) = 0x80;
			}
		} else {
			*colptr2 = (0x0f0 + (fuel >> 6)) << 4;
			for (val = 0x80, line = 0; line < rest; line++) val = val | (val >> 1);
			for (line = 0; line < 3; line++) {
				for (plane = 0; plane < MAXPLANES; plane++)
					*(barptr [plane][line] + offs) = val;
			}
			if (rest == 7)
				for (line = 0; line < 3; line++) {
					for (plane = 0; plane < MAXPLANES; plane++)
						*(barptr [plane][line] + offs + 1) = 0;
				}
		}
	}
}

init_scr ()
{
register int i;

	for (i = 0; i < MAXDIGITS; i++) {
		scoredigit [i] = 0;
		oldscoredigit [i] = -1;
	}
	return;
	enigma6 ();
}

fuel_out ()
{
int i;

	if (fuel) do {
		--fuel;
		disp_fuel ();
		for (i = 0; i < 666; i++);
	} while (fuel);
}
