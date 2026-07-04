extern struct ExecBase *SysBase;
struct FileHandler *fh, *Open ();

extern long BitPlane2 [MAXPLANES];

long score [2];
char *scoreptr0 [MAXPLANES];
int scoreage [MAXDIGITS], scoredigit [MAXDIGITS], oldscoredigit [MAXDIGITS];

char chars [10][64];
int player=0;

load_chars ()
{
register int i;

	for (i = 0; i < MAXPLANES; i++) scoreptr0 [i] = (char *) BitPlane2 [i]+3*WIDTH/8+48/8;

	fh = Open ("GFX:chars", MODE_OLDFILE);
	for (i = 0; i < 10; i++) Read (fh, chars [i], 40L);
	Close (fh);
}

init_scr ()
{
register int i;

	load_chars ();

	for (i = 0; i < MAXDIGITS; i++) {
		scoredigit [i] = 0;
		oldscoredigit [i] = -1;
	}

	score [0] = score [1] = 0;
	new_score ();
}

new_score ()
{
register int i;
register long scr;

	if (scoreage [MAXDIGITS-1] == 8) {
		if ((scr = score [player]) > 90000L)
			scr = score [player] = 0L;
		scoredigit [0] = (scr % 100000L) / 10000L;
		scoredigit [1] = (scr % 10000L) / 1000L;
		scoredigit [2] = (scr % 1000L) / 100L;
		scoredigit [3] = (scr % 100L) / 10L;
		scoredigit [4] = scr % 10L;

		for (i = 0; i < MAXDIGITS; i++) {
			if (scoredigit [i] != oldscoredigit [i])
				scoreage [i] = 0;
			oldscoredigit [i] = scoredigit [i];
		}
	}
}

refresh_score ()
{
register char *ptr1;
register int line, plane;
register int digit, age;

	for (digit = 0; digit < MAXDIGITS; digit++)
		if ((age = scoreage [digit]) < 8) {
			for (plane = 0; plane < MAXPLANES; plane++) {
				ptr1 = scoreptr0 [plane] + digit;
				for (line = 0; line < 7; line++) {
					*ptr1 = *(ptr1 + WIDTH/8);
					ptr1 += WIDTH/8;
				}
				*ptr1 = chars [scoredigit [digit]][(plane<<3)+age];
			}
			++scoreage [digit];
		}
}
