struct FileHandler *fh, *Open ();
long Write ();

struct ExitDef {
	int x, y, a;
} exitpos [6] = {
	101, 8, -1
};

UWORD colortable [32] = {
    0x0,0xDB9,0xED2,0xCA2,0xC97,0xA75,0x123,0x78A,
    0x568,0x345,0x234,0xC21,0x922,0x522,0x211,0xC11,
    0x483,0x373,0x151,0x131,0xBEC,0x8C9,0x5B7,0x486,
    0x275,0x154,0x143,0x132,0x121,0xEFF,0x361,0x110
};

struct CompDef {
	int x, y, typ, files, accessmode [MAXDEVS];
} comp [MAXCOMPS] = {
	{ 4,  0, MSWITCH,   0, { 0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 4,  4, MSWITCH,   0, { 3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 4,  8, MSWITCH,   0, { 0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 50, 0, MCOMPUTER, 0, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 50, 8, MCOMPUTER, 3, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 54, 0, MCOMPUTER, 1, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 54, 8, MCOMPUTER, 4, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 58, 0, MCOMPUTER, 2, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 83, 8, MCOMPUTER, 5, { 0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 87, 0, MCOMPUTER, 6, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 91, 0, MCOMPUTER, 7, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 91, 8, MCOMPUTER, 8, { 0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 122,0, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0 } },
	{ 122,4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,0 } },
	{ 122,8, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0 } }
};
/*                          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0*/

struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS] = {
	{ 8,  0, 1, MAGRAV,  0, 6, 0, 0, 0, 0 },
	{ 8,  4, 1, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 8,  8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 14, 0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 14, 4, 0, MENERGY, 1, 0, 1*16*32L, 13*16*32L, 16*16*32L, 61*16*32L },
	{ 14, 8, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 62, 4, 1, MENERGY, 2, 0, 16*16*32L, 61*16*32L, 64*16*32L, 65*16*32L },
	{ 83, 4, 1, MENERGY, 3, 0, 81*16*32L, 82*16*32L, 85*16*32L, 110*16*32L },
	{ 101,8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 111,0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 111,4, 0, MENERGY, 4, 0, 85*16*32L, 110*16*32L, 113*16*32L, 125*16*32L },
	{ 111,8, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 117,0, 0, MAGRAV,  0, 7, 0, 0, 0, 0 },
	{ 117,4, 1, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 117,8, 1, MAGRAV,  0, 0, 0, 0, 0, 0 }
};

struct MyFiles {
	char *file [8];
} compfiles [MAXCOMPS] = {
	{	".DAYTKOR.TXT",
		".GORTARG.TXT",
		".BW74.TXT",
		" BW75.TXT",
		" BW76.TXT",
		" GHRORAK.TXT",
		" MAP.PIC",
		" " },
	{	".HYDHOR.TXT",
		" LAZY.TXT",
		" SIRIUS.TXT",
		" TITASH.TXT",
		" SOL.TXT",
		" MAP.PIC",
		" ",
		" " },
	{	".CAPUD.TXT",
		".NTIFIC.TXT",
		".BAYKATOSER.TXT",
		" MAP.PIC",
		" ",
		" ",
		" ",
		" " },
	{	" TARYNN.TXT",
		" PENETRATOR.PIC",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" NOTTRANS.TXT",
		" TRANSPOINT.PIC",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK8.TXT",
		" DISPLAY.PIC",
		" SHIP.PIC",
		" CODES.TXT",
		" ",
		" ",
		" ",
		" " },
	{	" HYGOR.TXT",
		" HYTEK.PIC",
		" MISSION.TXT",
		" AKTTASK.TXT",
		" CHECK.TXT",
		" ",
		" ",
		" " },
	{	" POSITION.PRG",
		" COURSE.PIC",
		" OBJECTS.TXT",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" CONNECTIONS.TXT",
		" MGARRYN.PRG",
		" USE.TXT",
		" DECKS.TXT",
		" ",
		" ",
		" ",
		" " }
};

struct FoeDef {
	long txl, txr;
	int y, v, img, mode;
} foedef [MAXFOES] = {
	{ 61*16*32L,  85*16*32L,  10*16*64,  20, 6, 32 },
	{ 1*16*32L,   61*16*32L,   6*16*64,  20, 4, 0 },
	{ 64*16*32L,  65*16*32L,   6*16*64,  20, 4, 0 },
	{ 81*16*32L,  82*16*32L,   6*16*64,  20, 7, 0 },
	{ 85*16*32L,  125*16*32L,  6*16*64,  20, 7, 0 }
};

main (argc, argv)
int argc;
char *argv [];
{
int i, j;

	fh = Open ("LEV:Struct7", MODE_NEWFILE);
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
