struct FileHandler *fh, *Open ();
long Write ();

struct ExitDef {
	int x, y, a;
} exitpos [6] = {
	98,  8, -1,
	116, 4, 1
};

UWORD colortable [32] = {
    0x0,0xDA9,0xEC2,0xC92,0xC87,0xA65,0x113,0x77A,
    0x558,0x335,0x224,0xE26,0xA13,0x701,0x300,0xF00,
    0x778,0x667,0x445,0x223,0xBCC,0x788,0x677,0x566,
    0x455,0x344,0x233,0x122,0x11,0xCCD,0x674,0x112
};

struct CompDef {
	int x, y, typ, files, accessmode [MAXDEVS];
} comp [MAXCOMPS] = {
	{ 7,  0, MCOMPUTER, 0, { 3,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 7,  4, MCOMPUTER, 0, { 3,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 7,  8, MCOMPUTER, 0, { 3,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 70, 0, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0 } },
	{ 70, 4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0 } },
	{ 77, 8, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0 } },
	{ 88, 4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 99, 4, MCOMPUTER, 1, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0 } },
	{ 110,0, MCOMPUTER, 2, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0 } },
	{ 110,4, MCOMPUTER, 3, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 117,0, MCOMPUTER, 4, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } }
};
/*                          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0*/

struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS] = {
	{ 2,  0, 0, MAGRAV,  0, 1, 0, 0, 0, 0 },
	{ 2,  4, 0, MAGRAV,  0, 2, 0, 0, 0, 0 },
	{ 2,  8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 16, 0, 1, MENERGY, 0, 6, 0, 0, 0, 0 },
	{ 16, 4, 1, MENERGY, 0, 7, 0, 0, 0, 0 },
	{ 16, 8, 1, MENERGY, 0, 8, 0, 0, 0, 0 },
	{ 66, 0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 66, 4, 1, MENERGY, 0, 8, 0, 0, 0, 0 },
	{ 66, 8, 0, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 80, 0, 0, MAGRAV,  0, 10,0, 0, 0, 0 },
	{ 80, 4, 0, MAGRAV,  0, 11,0, 0, 0, 0 },
	{ 80, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 88, 8, 1, MENERGY, 3, 13,85*16*32L, 87*16*32L, 90*16*32L, 95*16*32L },
	{ 92, 4, 0, MAGRAV,  0, 14,0, 0, 0, 0 },
	{ 92, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 98, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 116,4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 1,  0, 0, MFOE,    0, 0, 0, 0, 0, 0 },
	{ 1,  0, 0, MFOE,    1, 0, 0, 0, 0, 0 },
	{ 1,  0, 0, MFOE,    2, 0, 0, 0, 0, 0 },
	{ 86, 0, 1, MENERGY, 0, 0, 0, 0, 0, 0 }
};

struct MyFiles {
	char *file [8];
} compfiles [MAXCOMPS] = {
	{	" LAGER.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK6.TXT",
		" DECK8.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" BWR.TXT",
		" BTECH.PIC",
		" GORGHAN.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK7.TXT",
		" DISPLAY.PIC",
		" FILE.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" SOLAR.TXT",
		" ASSIMILATOR.PIC",
		" NOTE.TXT",
		" ",
		" ",
		" ",
		" ",
		" " }
};

struct FoeDef {
	long txl, txr;
	int y, v, img, mode;
} foedef [MAXFOES] = {
	{ 68*16*32L,  77*16*32L,  2*16*64,  64, 5, 17 },
	{ 68*16*32L,  77*16*32L,  6*16*64,  64, 5, 17 },
	{ 79*16*32L,  83*16*32L,  2*16*64,  42, 6, 40 },
	{ 90*16*32L,  95*16*32L, 10*16*64,  32, 5, 1  },
	{ 91*16*32L, 106*16*32L,  1*16*64,  32, 3, 2  },
	{ 91*16*32L, 106*16*32L,  1*16*64,  42, 3, 2  }
};

main (argc, argv)
int argc;
char *argv [];
{
int i, j;

	fh = Open ("LEV:Struct6", MODE_NEWFILE);
	printf ("%ld\n", (long) Write (fh, exitpos, (long) (sizeof (struct ExitDef) * 6)));
	printf ("%ld\n", (long) Write (fh, comp, (long) (sizeof (struct CompDef) * MAXCOMPS)));
	printf ("%ld\n", (long) Write (fh, device, (long) (sizeof (struct DevDef) * MAXDEVS)));

	for (i = 0; i < MAXCOMPS; i++)
		for (j = 0; j < 8; j++)
			Write (fh, compfiles [i].file [j], 20L);

	printf ("%ld\n", (long) Write (fh, foedef, (long) (sizeof (struct FoeDef) * MAXFOES)));
	printf ("%ld\n", (long) Write (fh, colortable, 64L));
	Close (fh);
}
