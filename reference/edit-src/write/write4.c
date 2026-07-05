struct FileHandler *fh, *Open ();
long Write ();

struct ExitDef {
	int x, y, a;
} exitpos [6] = {
	11, 4, -4,
	63, 8, 2
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
	{ 3,  0, MCOMPUTER, 0, { 3,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 3,  4, MCOMPUTER, 1, { 0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 39, 4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 52, 4, MCOMPUTER, 2, { 0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0 } },
	{ 52, 8, MCOMPUTER, 3, { 0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 112,4, MCOMPUTER, 4, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0 } },
	{ 112,8, MCOMPUTER, 5, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0 } }
};

struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS] = {
	{ 5,  4, 0, MAGRAV,  0, 1, 0, 0, 0, 0 },
	{ 5,  8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 11, 4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 15, 8, 1, MENERGY, 2, 0, 1*16*32L, 14*16*32L, 17*16*32L, 42*16*32L },
	{ 33, 0, 0, MAGRAV,  0, 5, 0, 0, 0, 0 },
	{ 33, 4, 0, MAGRAV,  0, 6, 0, 0, 0, 0 },
	{ 33, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 43, 4, 1, MENERGY, 3, 0, 17*16*32L, 42*16*32L, 45*16*32L, 54*16*32L },
	{ 47, 0, 0, MAGRAV,  0, 9, 0, 0, 0, 0 },
	{ 47, 4, 0, MAGRAV,  0,10, 0, 0, 0, 0 },
	{ 47, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 57, 4, 1, MAGRAV,  0,12, 0, 0, 0, 0 },
	{ 57, 8, 1, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 63, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 89, 4, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 89, 8, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 91, 4, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 91, 8, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 1,  0, 1, MFOE,    4, 0, 0, 0, 0, 0 }
};

struct MyFiles {
	char *file [8];
} compfiles [MAXCOMPS] = {
	{	" SECTION2.TXT",
		" DISPLAY.PIC",
		" GARRYNNCONECT.TXT",
		" MISSION.TXT",
		" AKTTASK.TXT",
		" ",
		" ",
		" " },
	{	" DECK1.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK5.TXT",
		" DISPLAY.PIC",
		" FILE.TXT",
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
	{	" NASKUR.TXT",
		" DETONATOR.PIC",
		" PANIKSHUT.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" MISSION.TXT",
		" AKTTASK.TXT",
		" ",
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
	{ 101*16*32L, 120*16*32L,  1*16*64,  40, 3, 2 },
	{ 101*16*32L, 120*16*32L,  1*16*64,  30, 3, 2 },
	{   1*16*32L,  14*16*32L, 10*16*64,  42, 4, 4 },
	{  45*16*32L,  54*16*32L,  6*16*64,  16, 7, 0 },
	{  56*16*32L,  68*16*32L,  6*16*64,  32, 5, 1 },
	{  95*16*32L, 105*16*32L,  5*16*64,  20, 1, 1 },
	{  95*16*32L, 105*16*32L,  6*16*64,  20, 1, 1 },
	{  95*16*32L, 105*16*32L,  9*16*64,  30, 1, 1 },
	{  95*16*32L, 105*16*32L,  10*16*64, 30, 1, 1 },
	{ 116*16*32L, 125*16*32L,  6*16*64,  32, 5, 1 },
	{ 116*16*32L, 125*16*32L,  6*16*64,  42, 5, 1 }
};

main (argc, argv)
int argc;
char *argv [];
{
int i, j;

	fh = Open ("LEV:Struct4", MODE_NEWFILE);
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
