struct FileHandler *fh, *Open ();
long Write ();

struct ExitDef {
	int x, y, a;
} exitpos [6] = {
	93,  8, 4,
	113, 4, 1
};

UWORD colortable [32] = {
    0x0,0xEA9,0xEC2,0xC92,0xC87,0xA65,0x113,0x77A,
    0x558,0x335,0x224,0x2BE,0x17A,0x146,0x12,0xD06,
    0xB78,0xA78,0x956,0x745,0x56C,0x449,0x326,0x203,
    0x534,0x434,0x323,0x112,0x213,0xAAF,0xA63,0x102
};

struct CompDef {
	int x, y, typ, files, accessmode [MAXDEVS];
} comp [MAXCOMPS] = {
	{ 7,  4, MCOMPUTER, 0, { 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 7,  8, MCOMPUTER, 1, { 0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 45, 8, MSWITCH,   0, { 0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 47, 0, MSWITCH,   0, { 0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 47, 4, MCOMPUTER, 2, { 0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 52, 8, MSWITCH,   0, { 0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 73, 4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 88, 0, MCOMPUTER, 3, { 0,0,0,0,0,0,0,0,3,0,0,0,3,0,0,0,0,0,0,0 } },
	{ 88, 8, MCOMPUTER, 4, { 0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0 } },
	{ 93, 0, MCOMPUTER, 5, { 0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,0,0 } },
	{ 101,4, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0 } },
	{ 113,0, MSWITCH,   0, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0 } }
};

struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS] = {
	{ 9,  4, 0, MAGRAV,  0, 1, 0, 0, 0, 0 },
	{ 9,  8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 13, 8, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 42, 0, 0, MAGRAV,  0, 4, 0, 0, 0, 0 },
	{ 42, 4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 48, 8, 1, MENERGY, 1, 0, 37*16*32L, 47*16*32L, 50*16*32L, 62*16*32L },
	{ 56, 0, 1, MAGRAV,  0, 7, 0, 0, 0, 0 },
	{ 56, 4, 1, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 73, 0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 85, 4, 0, MAGRAV,  0, 10,0, 0, 0, 0 },
	{ 85, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 101,0, 1, MENERGY, 0, 0, 0, 0, 0, 0 },
	{ 113,4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 93, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 117,0, 1, MENERGY, 4, 0, 111*16*32L, 116*16*32L, 119*16*32L, 122*16*32L },
};

struct MyFiles {
	char *file [8];
} compfiles [MAXCOMPS] = {
	{	" DECK1.TXT",
		" DISPLAY.PIC",
		".CODES.TXT",
		" ",
		" ",
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
	{	" BWR.TXT",
		" BTECH.PIC",
		" DNX.TXT",
		" DTECH.PIC",
		" DORPUR.TXT",
		" DURKAH.TXT",
		" README.TXT",
		" " },
	{	" DECK2.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" YRK.TXT",
		" YTECH.PIC",
		" VCT.TXT",
		" VTECH.PIC",
		" GARRYNN.TXT",
		" PERYNN.TXT",
		" ",
		" " },
	{	" DECK5.TXT",
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
	{   7*16*32L,  20*16*32L,    6*16*64,  20, 4, 0 },
	{  37*16*32L,  47*16*32L,   10*16*64,  20, 4, 4 },
	{  69*16*32L, 106*16*32L,    6*16*64,  32, 4, 0 },
	{  92*16*32L, 110*16*32L,    9*16*64,  24, 2, 0 },
	{ 119*16*32L, 122*16*32L,    2*16*64,  16, 7, 4 },
	{  21*16*32L,  34*16*32L,    1*16*64,  30, 3, 2 },
	{  21*16*32L,  34*16*32L,    1*16*64,  40, 3, 2 },
	{  71*16*32L,  82*16*32L,   10*16*64,  40, 6,32 },
	{ 112*16*32L, 122*16*32L,    7*16*64,  24, 2, 0 }
};

main (argc, argv)
int argc;
char *argv [];
{
int i, j;

	fh = Open ("LEV:Struct0", MODE_NEWFILE);
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
