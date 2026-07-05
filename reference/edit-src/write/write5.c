struct FileHandler *fh, *Open ();
long Write ();

struct ExitDef {
	int x, y, a;
} exitpos [6] = {
	6,   4, -1,
	34,  4, 1
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
	{ 6,  8, MSWITCH,   0, { 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 17, 4, MCOMPUTER, 0, { 0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 25, 4, MCOMPUTER, 1, { 0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 36, 8, MSWITCH,   0, { 0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 64, 0, MCOMPUTER, 2, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 64, 4, MCOMPUTER, 3, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 81, 4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 81, 8, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 86, 4, MCOMPUTER, 4, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 123,4, MCOMPUTER, 5, { 0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } }
};
/*                          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0*/

struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS] = {
	{ 6,  0, 0, MENERGY, 0, 6, 0, 0, 0, 0 },
	{ 6,  4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 20, 0, 0, MAGRAV,  0, 3, 0, 0, 0, 0 },
	{ 20, 4, 0, MAGRAV,  0,13, 0, 0, 0, 0 },
	{ 36, 0, 0, MENERGY, 0, 8, 0, 0, 0, 0 },
	{ 34, 4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 48, 0, 0, MAGRAV,  0, 7, 0, 0, 0, 0 },
	{ 48, 4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 48, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 76, 0, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 76, 4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 76, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 1,  0, 0, MFOE,    0, 0, 0, 0, 0, 0 },
	{ 1,  0, 0, MFOE,    7, 0, 0, 0, 0, 0 }
};

struct MyFiles {
	char *file [8];
} compfiles [MAXCOMPS] = {
	{	" DECK5.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK7.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" CLIMATE.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" TARYN.TXT",
		" PENETRATOR.PIC",
		" DIRECTIVE.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" INTERNORT.TXT",
		" TASK.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK6.TXT",
		" DISPLAY.PIC",
		" SYMBOL.TXT",
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
	{ 2*16*32L,  88*16*32L,  10*16*64,128, 6, 40 },
	{ 2*16*32L,  88*16*32L,  10*16*64, 64, 7, 0 },
	{ 2*16*32L,  88*16*32L,  10*16*64, 42, 4, 0 },
	{ 47*16*32L, 66*16*32L,   6*16*64, 50, 5, 1 },
	{ 47*16*32L, 66*16*32L,   6*16*64, 40, 5, 1 },
	{ 47*16*32L, 66*16*32L,   6*16*64, 30, 7, 0 },
	{ 47*16*32L, 66*16*32L,   2*16*64, 16, 4, 0 },
	{ 73*16*32L,105*16*32L,   6*16*64, 32, 5,17 }
};

main (argc, argv)
int argc;
char *argv [];
{
int i, j;

	fh = Open ("LEV:Struct5", MODE_NEWFILE);
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
