struct FileHandler *fh, *Open ();
long Write ();

struct ExitDef {
	int x, y, a;
} exitpos [6] = {
	28, 4, -1
};

UWORD colortable [32] = {
    0x0,0xFB9,0xDC2,0xBA2,0xD97,0xB75,0x223,0x99A,
    0x778,0x556,0x334,0xFFF,0xBBB,0x666,0x222,0xF00,
    0x6F9,0x4B9,0x288,0x134,0xCEE,0xACD,0x8AB,0x68A,
    0x469,0x348,0x237,0x225,0x214,0xDFF,0x387,0x103
};

struct CompDef {
	int x, y, typ, files, accessmode [MAXDEVS];
} comp [MAXCOMPS] = {
	{ 2,  4, MCOMPUTER, 0, { 0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 8,  8, MSWITCH,   0, { 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 26, 8, MSWITCH,   0, { 0,0,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 46, 4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 46, 8, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 50, 0, MCOMPUTER, 1, { 0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 65, 4, MCOMPUTER, 2, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 75, 4, MCOMPUTER, 3, { 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 91, 4, MCOMPUTER, 4, { 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 114,0, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 114,4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } }
};
/*                          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0*/

struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS] = {
	{ 4,  8, 0, MENERGY, 0, 1, 0, 0, 0, 0 },
	{ 12, 4, 0, MENERGY, 0, 2, 0, 0, 0, 0 },
	{ 12, 8, 0, MENERGY, 0, 3, 0, 0, 0, 0 },
	{ 22, 4, 0, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 22, 8, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 28, 4, 0, MAGRAV,  0, 3, 0, 0, 0, 0 },
	{ 30, 8, 1, MENERGY, 0, 10,0, 0, 0, 0 },
	{ 40, 8, 0, MENERGY, 0, 10,0, 0, 0, 0 },
	{ 42, 4, 1, MENERGY, 0, 10,0, 0, 0, 0 },
	{ 44, 0, 1, MENERGY, 0, 8, 0, 0, 0, 0 },
	{ 50, 4, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 50, 8, 1, MENERGY, 0, 7, 0, 0, 0, 0 },
	{ 117,0, 1, MENERGY, 3, 13,113*16*32L, 116*16*32L, 119*16*32L, 125*16*32L },
	{ 111,0, 0, MENERGY, 3, 0 ,108*16*32L, 110*16*32L, 113*16*32L, 116*16*32L },
	{ 119,4, 0, MAGRAV,  0, 15,0, 0, 0, 0 },
	{ 119,8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 }
};

struct MyFiles {
	char *file [8];
} compfiles [MAXCOMPS] = {
	{	" DECK2.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK3.TXT",
		" DISPLAY.PIC",
		" FILE.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	".SECTION3.TXT",
		" DISPLAY.PIC",
		" GARRYNNCONECT.TXT",
		" MISSION.TXT",
		" AKTTASK.TXT",
		" ",
		" ",
		" " },
	{	" TACHYONEN.TXT",
		" DESOLATOR.PIC",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" SEMORCOM.TXT",
		" TECHNIK.PIC",
		" ORDER.TXT",
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
	{ 1*16*32L,   34*16*32L,   1*16*64,  10, 1, 1 },
	{ 1*16*32L,   34*16*32L,   2*16*64+512,  10, 1, 1 },
	{ 19*16*32L,  34*16*32L,   1*16*64,  43, 2, 1 },
	{ 119*16*32L, 125*16*32L,  2*16*64,  32, 1, 5 },
	{ 58*16*32L,  106*16*32L,  9*16*64,  30, 6, 32 },
	{ 58*16*32L,  106*16*32L,  10*16*64+512, 30, 6, 32 },
	{ 74*16*32L,  106*16*32L,  1*16*64,  30, 2, 1 },
	{ 74*16*32L,  106*16*32L,  2*16*64+512,  30, 2, 1 }
};

main (argc, argv)
int argc;
char *argv [];
{
int i, j;

	fh = Open ("LEV:Struct2", MODE_NEWFILE);
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
