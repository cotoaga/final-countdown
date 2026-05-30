extern struct FoeDef {
	long txl, txr;
	int y, v, img, mode;
} foedef [MAXFOES];

extern WORD foesperlev [MAXLEVELS];

WORD foes, expfn, expt;
WORD foeshp [MAXFOES], visib [MAXFOES], foeadr [MAXFOES], foeimg [MAXFOES];
WORD foef [MAXFOES], foed [MAXFOES], foev [MAXFOES], foey [MAXFOES];
LONG foex [MAXFOES], foetxl [MAXFOES], foetxr [MAXFOES];

kill_all_foes ()
{
register int i;

	for (i = 0; i < foes; i++) {
		if (visib [i]) rem_sprite (foeadr [i]);
		foex [i] = visib [i] = 0;
	}
}

kill_foe (no)
register int no;
{
	if (visib [no]) rem_sprite (foeadr [no]);
	foex [no] = visib [no] = 0;
}

init_foes (level)
int level;
{
register int i;

	for (i = 0; i < foes; i++) {
		foed [i] = 1;
		visib [i] = foeshp [i] = 0;
		foev [i] = foedef [i].v;
		foey [i] = foedef [i].y;
		foef [i] = foedef [i].mode;
		foeimg [i] = foedef [i].img;
		foetxr [i] = foedef [i].txr;
		foex [i] = foetxl [i] = foedef [i].txl;
		if (foef [i] & 8) {
			foex [i] = foedef [i].txl;
			foed [i] = 0;
		}
		if (foef [i] & 16) {
			foex [i] = foedef [i].txr;
			foed [i] = 0;
		}
	}
	expfn = -1; expt = 0;
	return;
	enigma3 ();
}
