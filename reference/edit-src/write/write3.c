struct FileHandler *fh, *Open ();
long Write ();

struct ExitDef {
	int x, y, a;
} exitpos [6] = {
	7,  4, -1,
	65, 8, -1
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
	{ 39, 4, MCOMPUTER, 0, { 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 65, 0, MCOMPUTER, 1, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 65, 4, MCOMPUTER, 2, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
	{ 118,0, MCOMPUTER, 3, { 0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } }
};

struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS] = {
	{ 7,  4, 0, MAGRAV,  0, 0, 0, 0, 0, 0 },
	{ 65, 8, 0, MAGRAV,  0, 0, 0, 0, 0, 0 }
};

struct MyFiles {
	char *file [8];
} compfiles [MAXCOMPS] = {
	{	" DECK3.TXT",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" QUINTRINO.TXT",
		" WANDLER.PIC",
		" ",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" EKIN.TXT",
		" MANIPULATOR.PIC",
		" DERUREKIN.PRG",
		" ",
		" ",
		" ",
		" ",
		" " },
	{	" DECK3.TXT",
		" DECK4.TXT",
		" DISPLAY.PIC",
		" CODES.TXT",
		" ",
		" ",
		" ",
		" " }
};

struct FoeDef {
	long txl, txr;
	int y, v, img, mode;
} foedef [MAXFOES] = {
};

main (argc, argv)
int argc;
char *argv [];
{
int i, j;

	fh = Open ("LEV:Struct3", MODE_NEWFILE);
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
