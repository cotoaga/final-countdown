struct FileHandler *fh, *Open ();
long Write ();

struct ExitDef {
	int x, y, a;
} exitpos [6] = {
	28, 8, 1,
	117,4, -1
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
	{ 12, 4, MSWITCH,   0, { 0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 20, 4, MSWITCH,   0, { 0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0 } },
	{ 29, 4, MCOMPUTER, 0, { 3,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0 } },
	{ 41, 0, MCOMPUTER, 1, { 0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 41, 4, MCOMPUTER, 2, { 0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,0,3,0,0,0,0,0,0,0,0,0,0 } },
	{ 46, 0, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 46, 4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 78, 0, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 78, 4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 84, 0, MCOMPUTER, 3, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0 } },
	{ 84, 4, MCOMPUTER, 4, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 103,0, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0 } },
	{ 103,4, MCOMPUTER, 5, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,3,0,0,0,0,0 } }
};
/*                          0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0*/

struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS] = {
	{ 2,  4, 0, MAGRAV,  0, 1, 0, 0, 0, 0 },
	{ 2,  8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 20, 0, 1, MENERGY, 2, 0, 14*16*32L, 19*16*32L, 22*16*32L, 23*16*32L },
	{ 12, 0, 0, MENERGY, 2, 0, 6*16*32L, 11*16*32L, 14*16*32L, 19*16*32L },
	{ 4,  0, 0, MENERGY, 2, 0, 1*16*32L, 3*16*32L, 6*16*32L, 11*16*32L },
	{ 29, 0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 28, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 38, 0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 38, 4, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 38, 8, 0, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 46, 8, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 78, 8, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 86, 0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 86, 4, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 86, 8, 0, MENERGY, 0,13, 0, 0, 0, 0 },
	{ 92, 0, 0, MAGRAV,  0,17, 0, 0, 0, 0 },
	{ 92, 4, 0, MAGRAV,  0,17, 0, 0, 0, 0 },
	{ 92, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 109,0, 0, MAGRAV,  0,19, 0, 0, 0, 0 },
	{ 109,4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 117,4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 119,0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 1,  0, 1, MFOE,    1, 0, 0, 0, 0, 0 },
	{ 1,  0, 0, MFOE,    0, 0, 0, 0, 0, 0 }
};

struct MyFiles {
	char *file [8];
} compfiles [MAXCOMPS] = {
	{	" DECK3.TXT",
		" GRB.TXT",
		" GTECH.PIC",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK2.TXT",
		" DISPLAY.PIC",
		".CODES.TXT"
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" THORRANUR.TXT",
		" DNY.TXT",
		" DTECH.PIC",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" RLL.TXT",
		" RTECH.PIC",
		" VCT.TXT",
		" VTECH.PIC",
		" TASK.TXT",
		" ",
		" ",
		" " },
	{	" VICHOROFF.TXT",
		" NOTTRANS.TXT",
		" ",
		" ",
		" ",
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
		" " }
};

struct FoeDef {
	long txl, txr;
	int y, v, img, mode;
} foedef [MAXFOES] = {
	{  19*16*32L,  31*16*32L,    6*16*64,  64, 4,16 },
	{  27*16*32L,  31*16*32L,   10*16*64,  32, 5, 1 },
	{  22*16*32L,  23*16*32L,    2*16*64,  24, 1, 5 },
	{   1*16*32L,   9*16*32L,    6*16*64,  24, 7, 0 },
	{   1*16*32L,   9*16*32L,    6*16*64,  32, 7, 0 },
	{  92*16*32L,  96*16*32L,    6*16*64,  20, 1, 1 },
	{ 105*16*32L, 120*16*32L,    9*16*64,  32, 2, 1 },
	{ 105*16*32L, 120*16*32L,   11*16*64,  24, 2, 1 }
};

main (argc, argv)
int argc;
char *argv [];
{
int i, j;

	fh = Open ("LEV:Struct1", MODE_NEWFILE);
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
