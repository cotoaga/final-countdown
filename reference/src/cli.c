#define VERSNO 3

extern struct CompDef {
	int x, y, typ, files, accessmode [MAXDEVS];
} comp [MAXCOMPS];
extern struct DevDef {
	int x, y, on, typ, spec, next;
	long xl1, xr1, xl2, xr2;
} device [MAXDEVS];
extern struct MyFiles {
	char file [8][20];
} compfiles [MAXCOMPS];
extern struct DiskFiles {
	char *file [8];
} diskfiles [8];
extern struct FoeDef {
	long txl, txr;
	int y, v, img, mode;
} foedef [MAXFOES];
extern struct StartDef {
	int x [2], y [2];
} startdef [MAXLEVELS];
extern struct ExitDef {
	int x, y, a;
} exitpos [6];

extern char keycode[];
extern LONG gtime;
extern WORD procom, master, fuel, disk, path, zahl, no, devstatus [MAXDEVS], dark;
extern WORD exno, statr, statg, statb, message, mines, devs, leveladd, level;
extern BYTE word1[], word2[], word3[];

extern int devicestatus [MAXLEVELS][MAXDEVS];

extern WORD intal, tarpen, viccol, hygkoo, ekiman, novbom, steexc, tacdes, semcom;
extern LONG ttime, itime, score;
extern WORD dka, dkb, dkc, mass, seldes, foes, gorg, fatal, xray, green, foef [MAXFOES], foed [MAXFOES];

long waitcount, VBeamPos ();

char *led = 0xBFE001;

BYTE *passfile [MAXLEVELS] = {
	"DOPHAZ",
	"ITLARS",
	"DESKIDE",
	"F4",
	"F5",
	"F6",
	"F7",
	"CORMUR"
};
BYTE *passcomp [MAXLEVELS] = {
	"NASKUR",
	"WPERYOD",
	"AXYRN",
	"SARDANYX",
	"CUTHUR",
	"NASKUR",
	"ENOBYNLEM",
	"KALATORYE"
};

BYTE *chklin1 [4] = {
	"WIEVIELE BEFEHLE SIND IN DER",
	"WIE LAUTET DER ERSTE BEFEHL IN",
	"WIE LAUTET DER LETZTE BEFEHL",
	"WIE LAUTET DER ERSTE BEFEHL"
};
BYTE *chklin2 [4] = {
	"KURZUEBERSICHT AUFGEFUEHRT ?  S1",
	"DER KURZUEBERSICHT ?  S1",
	"IN DER KURZUEBERSICHT ?  S1",
	"OBEN AUF SEITE 7 ?"
};
BYTE *chklin3 [4] = {
	"13",
	"CD",
	"ENDCLI",
	"SYSTEM"
};

LONG AvailMem ();
long atol ();
BYTE *get_line ();

BYTE accden [] = "ACCESS DENIED !+";
BYTE pwait1 [] = "PLEASE WAIT.";
BYTE pwait2 [] = "TRANSLATION IN PROGRESS+";
BYTE profile [] = "PROTECTED FILE !";
BYTE undevt [] = "UNKNOWN DEVICE TYPE+";
BYTE afwa [] = "A FILENAME WITHOUT APPENDIX+";
BYTE noab [] = "THIS COMPUTER IS NOT ABLE";
BYTE toru [] = "TO RUN THIS SOFTWARE !+";
BYTE new1 [] = "MISSION CONTROLER IS";
BYTE new2 [] = "COMPUTING NEW AKTTASK.TXT !+";

char *autcodedev [MAXLEVELS] = {
	"ACCESS CODE DECK 1 ?",
	"ACCESS CODE DECK 2 ?",
	"TACH.DES AUTHORISATION SYMBOL ?",
	"EKIN.MAN AUTHORISATION SYMBOL ?",
	"ENTER PERSONAL IDENTIFIKATION !",
	"ACCESS CODE DECK1 ?",
	"ENTER KEY aabbbcccc+",
	"MGARRYNN ACCESS SYMBOL ?",
};

char *autcodefil [MAXLEVELS] = {
	"SECTION III CODE abc ?",
	"SECTION III CODE def ?",
	"CHECK SYMBOL OF BETAH H5259 ?",
	"ENTER F4 CODE+",
	"ENTER F5 CODE+",
	"ENTER F6 CODE+",
	"ENTER F7 CODE+",
	"NAME OF NAVIGATION SYSTEM ?",
};

int versno = VERSNO;

wait_loop (time)
int time;
{
long count, end;

	for (count = 0, end = time * 1000L; count < end; count++);
}

put_whwtd (typ, no, action)
int typ, no, action;
{
	if (action) switch (typ) {
		case MENERGY:	sprintf (word1, "GATE %d ENABLED+", no);
							do_sound (13);
							break;
		case MFOE:		sprintf (word1, "ROBOT %d ENABLED+", no);
							do_sound (13);
							break;
		case MAGRAV:	sprintf (word1, "LIFT %d ENABLED+", no);
							do_sound (13);
	} else switch (typ) {
		case MENERGY:	sprintf (word1, "GATE %d DISABLED+", no);
							do_sound (14);
							break;
		case MFOE:		sprintf (word1, "ROBOT %d DISABLED+", no);
							do_sound (14);
							break;
		case MAGRAV:	sprintf (word1, "LIFT %d DISABLED+", no);
							do_sound (14);
	}
	put_line (word1);
}

put_fatal ()
{
int i;

	do_sound (12);
	do_sound (2);
	do_sound (2);
	put_line ("FATAL OPERATION ATEMPTED");
	put_line ("HYGOR HAS INITIATED");
	put_line ("CLASS II INTRUDER ALERT !");
	put_line ("NASKUR-DETONATOR IGNITION");
	put_line ("SEQUENCE IS RUNNING !");
	put_line ("IGNITION TIME IN");
	put_line ("10 TIME UNITS !+");
	if (itime < 9990) itime = 9990;
	set_yellow ();
}

check_user ()
{
int i;

	if (fatal) {
		put_line ("HYGOR OVERRIDE ORDER:");
		put_line ("NO MORE EXTERNAL COMPUTER");
		put_line ("ACCESS ALLOWED !");
		wait_loop (50);
		put_line ("");
		put_line ("INTRUDER ALERT CLASS II");
		put_line ("+");
		wait_loop (100);
		return 1;
	}
	if (procom) {
		procom = 0;
		do_sound (0);
		put_line ("PROTECTED DEVICE");
		put_line (autcodedev [level]);
		if (!master && strcmp (passcomp [level], (char *) get_line ())) {
			if (!intal) {
				do_sound (12);
				intal = 1;
			}
			put_line ("INTRUDER ALERT CLASS I");
			put_line ("");
			put_line ("10 TIME UNITS UNTIL CLASS II");
			put_line ("+");
			return 1;
		} else {
			put_line ("+");
			return 0;
			strcmp (&no, get_line ());
		}
	}
	return 0;
}

handle_system ()
{
static int havebeenhere = 0;
register int i, ii;
register char *ptr;
long cvar;

/*	if (!havebeenhere) {
		havebeenhere = 1;
		i = (VBeamPos () & 3);
		put_line (chklin1 [i]);
		put_line (chklin2 [i]);
		if (strcmp (chklin3 [i], (char *) get_line ())) fatal = 1;
		put_line ("+");
	}*/
	if (!strcmp (word1 , "ENABLE")) {
		if (check_user ()) return 1;
		if (!strcmp (word2, "ALPHA2")) {
			put_line ("WELCOME MASTER+");
			if( zahl != -1 ) leveladd = zahl - level;
			master = 1;
			return 0;
		}
		if (!strcmp (word2, "GATE")) {
			if (zahl == -1) {
				put_line ("WHICH GATE ?+");
				return 0;
			}
			if (device [zahl].typ == MENERGY) {
				if (devstatus [zahl]) {
					put_line ("GATE IS ALREADY ENABLED+");
					return 0;
				}
				if (comp [no].accessmode [zahl] & 1) {
					put_whwtd (MENERGY, zahl, 1);
					dev_on (zahl);
				} else {
					put_line (accden);
				}
			} else {
				put_line ("NO SUCH GATE+");
			}
			return 0;
		}
		if (!strcmp (word2, "ROBOT")) {
			if (zahl == -1) {
				put_line ("WHICH ROBOT ?+");
				return 0;
			}
			if (device [zahl].typ == MFOE) {
				if (devstatus [zahl]) {
					put_line ("ROBOT IS ALREADY ENABLED+");
					return 0;
				}
				if (comp [no].accessmode [zahl] & 1) {
					put_whwtd (MFOE, zahl, 1);
					dev_on (zahl);
				} else {
					put_line (accden);
				}
			} else {
				put_line ("NO SUCH ROBOT+");
			}
			return 0;
		}
		if (!strcmp (word2, "LIFT")) {
			if (zahl == -1) {
				put_line ("WHICH LIFT ?+");
				return 0;
			}
			if (device [zahl].typ == MAGRAV) {
				if (devstatus [zahl]) {
					put_line ("LIFT IS ALREADY ENABLED+");
					return 0;
				}
				if (comp [no].accessmode [zahl] & 1) {
					put_whwtd (MAGRAV, zahl, 1);
					dev_on (zahl);
/*					if (level == 3) enigma4 ();*/
				} else {
					put_line (accden);
				}
			} else {
				put_line ("NO SUCH LIFT+");
			}
			return 0;
		}
		if (!strcmp (word2, "ALL")) {
			for (i = 0; i < devs; i++)
				if (comp [no].accessmode [i] & 1 && !devstatus [i]) {
					put_whwtd (device [i].typ, i, 1);
/*					if (level == 3) enigma4 ();*/
					dev_on (i);
					for (cvar = 0; cvar < 54321; cvar++);
				}
			return 0;
		}
		put_line (undevt);
		return 0;
	}
	if (!strcmp (word1 , "DISABLE")) {
		if (check_user ()) return 1;
		if (!strcmp (word2, "ALPHA2")) {
			master = 0;
			return 0;
		}
		if (!strcmp (word2, "GATE")) {
			if (zahl == -1) {
				put_line ("WHICH GATE ?+");
				return 0;
				strcmp (&zahl, get_line ());
			}
			if (device [zahl].typ == MENERGY) {
				if (!devstatus [zahl]) {
					put_line ("GATE IS ALREADY DISABLED+");
					return 0;
				}
				if (comp [no].accessmode [zahl] & 2) {
					put_whwtd (MENERGY, zahl, 0);
					dev_off (zahl);
				} else {
					put_line (accden);
				}
			} else {
				put_line ("NO SUCH GATE+");
			}
			return 0;
		}
		if (!strcmp (word2, "ROBOT")) {
			if (zahl == -1) {
				put_line ("WHICH ROBOT ?+");
				return 0;
			}
			if (device [zahl].typ == MFOE) {
				if (!devstatus [zahl]) {
					put_line ("ROBOT IS ALREADY DISABLED+");
					return 0;
				}
				if (comp [no].accessmode [zahl] & 2) {
					put_whwtd (MFOE, zahl, 0);
					dev_off (zahl);
/*					enigma2 ();*/
				} else {
					put_line (accden);
				}
			} else {
				put_line ("NO SUCH ROBOT+");
			}
			return 0;
		}
		if (!strcmp (word2, "LIFT")) {
			if (zahl == -1) {
				put_line ("WHICH LIFT ?+");
				return 0;
			}
			if (device [zahl].typ == MAGRAV) {
				if (!devstatus [zahl]) {
					put_line ("LIFT IS ALREADY DISABLED+");
					return 0;
				}
				if (comp [no].accessmode [zahl] & 2) {
					put_whwtd (MAGRAV, zahl, 0);
					dev_off (zahl);
				} else {
					put_line (accden);
				}
			} else {
				put_line ("NO SUCH LIFT+");
			}
			return 0;
		}
		if (!strcmp (word2, "ALL")) {
			for (i = 0; i < devs; i++)
				if (comp [no].accessmode [i] & 2 && devstatus [i]) {
					put_whwtd (device [i].typ, i, 0);
					dev_off (i);
					for (cvar = 0; cvar < 54321; cvar++);
				}
			return 0;
			}
		put_line (undevt);
		return 0;
	}
	if( !strcmp( word1 , "LIST" ) ) {
		for (ii = i = 0; i < devs; i++) ii += comp [no].accessmode [i];
		if (!ii) {
			put_line ("NO DIRECT ACCESSED DEVICES+");
			return 0;
		}
		do_sound (16);
		put_line( "DEVICE STATUS:" );
		strcpy( word1, "       /9 " );
		for( i = 0; i < devs; i++ ) {
			switch (device [i].typ) {
				case MENERGY:	strncpy (word1, " GATE  ", 7);
									break;
				case MFOE:		strncpy (word1, " ROBOT ", 7);
									break;
				case MAGRAV:	strncpy (word1, " LIFT  ", 7);
			}
			if( devstatus [i] ) strcpy( word1+10, " ON  " );
			else strcpy( word1+10, " OFF " );
			if( ++ word1[8] > 0x39 ) {
				word1[8] = 0x30;
				word1[7] ++;
			}
			switch (comp [no].accessmode [i]) {
				case 1:	strcpy (word1+15, "E");
							break;
				case 2:	strcpy (word1+15, "D");
							break;
				case 3:	strcpy (word1+15, "ED");
			}
			if (!comp [no].accessmode [i]) continue;
			put_line (word1);
		}
		put_line ("+");
		kill_chan_with_pri (13);
		return 0;
	}
	if (!strcmp (word1 , "STATUS")) {
		if (check_user ()) return 1;
		do_sound (16);
		sprintf (word1, " MISSION TIME        %ld", score);
		put_line (word1);
		if (itime) {
			if (steexc) sprintf (word1, " IGNITION TIME IN    %d", (200 - steexc));
			else sprintf (word1, " IGNITION TIME       %ld", (10000-itime)+score);
			put_line (word1);
		}
		strcpy (word1, " TARYN-PENETRATOR    ACTIVE");
		if (tarpen) strcpy (word1+21, "OVERHEATING");
		if (viccol) strcpy (word1+21, "DESTROYED");
		put_line (word1);
		strcpy (word1, " VICHOROFF-COLLECTOR ACTIVE");
		if (viccol) strcpy (word1+21, "RESERVE");
		put_line (word1);
		strcpy (word1, " HYGOR-KOORDINATOR   RUNNING");
		if (hygkoo) strcpy (word1+21, "DISABLED");
		put_line (word1);
		strcpy (word1, " EKIN-MANIPULATOR    READY");
		if (steexc) strcpy (word1+21, "ACCELERATING");
		if (ekiman) strcpy (word1+21, "STOPPED");
		put_line (word1);
		strcpy (word1, " NASKUR-DETONATOR    READY");
		if (novbom) strcpy (word1+21, "WORKING");
		if (seldes) strcpy (word1+21, "ABORTED");
		put_line (word1);
		strcpy (word1, " SOLAR-ASSIMILATOR   WORKING");
		if (tacdes && !ekiman) strcpy (word1+21, "MAXIMUM POWER");
		if (steexc || ekiman) strcpy (word1+21, "OFF");
		put_line (word1);
		strcpy (word1, " TACHYONEN-DESOLATOR WORKING");
		if (tacdes) strcpy (word1+21, "OFF");
		put_line (word1);
		strcpy (word1, " SEMORCOM            READY");
		if (semcom) strcpy (word1+21, "OUT OF ORDER");
		put_line (word1);
		if (seldes) {
			put_line ("");
			sprintf (word1, " SELFDESTRUCTION IN  %ld+", (long)(200L-seldes));
			put_line (word1);
		}
		put_line ("+");
		kill_chan_with_pri (13);
		return 0;
	}
	if (!strcmp (word1 , "MEM")) {
		sprintf (word1, " CHIP %ld", AvailMem (MEMF_CHIP));
		put_line (word1);
		sprintf (word1, " FAST %ld", AvailMem (MEMF_FAST));
		put_line (word1);
		return 0;
	}
	if (!strcmp (word1 , "HELP")) {
		if (!strlen (word2)) {
			do_sound (16);
			put_line ("COMMAND LIST");
			put_line ("");
			put_line (" TYPE   SHOW    CLS    CD  DIR");
			put_line (" LIST   SYSTEM  STATUS");
			put_line (" ENABLE DISABLE");
			put_line (" HELP   INFO");
			put_line (" HIFI   SETMAP");
			put_line (" ENDCLI");
			put_line ("");
			put_line ("TYPE HELP FOLLOWED BY A COMMAND");
			put_line ("FOR MORE INFORMATION+");
			kill_chan_with_pri (13);
			return 0;
		}
		if (!strcmp (word2, "LIST")) {
			put_line (" PRINTS A LIST OF ALL ACCESSIBLE");
			put_line (" DEVICES AND THEIR ACTUAL STATUS.");
			put_line (" YOU CAN EITHER -E ENABLE THEM");
			put_line (" OR -D DISABLE THEM");
			put_line (" OR DO -ED BOTH+");
			return 0;
		}
		if (!strcmp (word2, "SYSTEM")) {
			put_line (" DESCRIBES SYSTEM HARDWARE");
			put_line (" AND CONTROL TASKS+");
			return 0;
		}
		if (!strcmp (word2, "STATUS")) {
			put_line (" LISTS ACTUAL STATUS OF THE");
			put_line (" MOST RELEVANT SYSTEMS.+");
			return 0;
		}
		if (!strcmp (word2, "ENABLE")) {
			put_line (" USAGE: -DEVICETYPE -NUMBER/ALL" );
			put_line (" SWITCHES THE ACCESSED DEVICE ON.");
			put_line (" NUMBER CAN BE REPLACED BY ALL.+");
			return 0;
		}
		if (!strcmp (word2, "DISABLE")) {
			put_line (" USAGE: -DEVICETYPE -NUMBER/ALL" );
			put_line (" SWITCHES OFF ACCESSED DEVICE");
			put_line (" USE  ALL  FOR ALROUND ACCESS+");
			return 0;
		}
		if (!strcmp (word2, "INFO")) {
			put_line (" GIVES YOU A BRIEF INVENTORY+");
			return 0;
		}
		if (!strcmp (word2, "TYPE")) {
			put_line (" USAGE: -TEXTFILE WITHOUT EXT" );
			put_line (" PRINTS TEXT FILES+");
			return 0;
		}
		if (!strcmp (word2, "MEM")) {
			put_line (" AVAILABLE MEMORY OF HOST COMP+");
			return 0;
		}
		if (!strcmp (word2, "SHOW")) {
			put_line (" USAGE: -PICFILE WITHOUT EXT");
			put_line (" DISPLAYS PICTURE FILES+");
			return 0;
		}
		if (!strcmp (word2, "CLS")) {
			put_line (" CLEARS DISPLAY+");
			return 0;
		}
		if (!strcmp (word2, "CD")) {
			put_line (" USAGE: /SYSTEM/DISK -DISKNR");
			put_line (" CHANGES CURRENT DIRECTORY");
			put_line (" OR DISPLAYS CURRENT PATH IF");
			put_line (" FOLLOWED BY NOTHING+");
			return 0;
		}
		if (!strcmp (word2, "HIFI")) {
			put_line (" USAGE: ON/OFF");
			put_line (" SWITCHES HIFI MODE");
			put_line (" ON OR OFF+");
			return 0;
		}
		if (!strcmp (word2, "SETMAP")) {
			put_line (" USAGE: D/USA");
			put_line (" SWITCHES KEYMAP+");
			return 0;
		}
		if (!strcmp (word2, "DIR")) {
			put_line (" USAGE: /SYSTEM/DISK/ -DISKNR");
			put_line (" LISTS CURRENT DIRECTORY");
			put_line (" OR DISPLAYS THE CHOOSEN PATH");
			put_line (" LIKE   DIR DISK 3+");
			return 0;
		}
		if (!strcmp (word2, "ENDCLI")) {
			put_line (" TERMINATES ACCESS+");
			return 0;
		}
		put_line ("NO INFORMATION AVAILABLE+");
		return 0;
	}
	if (!strcmp (word1 , "DIR")) {
		do_sound (16);
		if (strlen (word2)) {
			ii = path;
			if (change_dir (0)) {
				kill_chan_with_pri (13);
				return 0;
			}
		} else ii = -1;
		if (!path) {
			if (fatal) put_line (" HYGORORDER.TXT");
			else {
				for (i = 0; i < 8; i++) {
					if (compfiles [comp [no].files].file [i][1])
						put_line (compfiles [comp [no].files].file [i]);
				}
				if (message) put_line (" MESSAGE.TXT");
			}
		} else if (path >= 1 && path <= 8) {
			for (i = 0; i < 8; i++) {
				if (diskfiles [path-1].file [i][1])
					put_line (diskfiles [path-1].file [i]);
			}
		}
		if (ii != -1) path = ii;
		kill_chan_with_pri (13);
		return 0;
	}
	if (!strcmp (word1, "SYSTEM")) {
		if (check_user ()) return 1;
		do_sound (17);
		put_line (pwait1);
		put_line (pwait2);
		put_line ("");
		strcpy (word1, "DISK01:TXT/Sys0A");
		word1 [14] += level;
		word1 [15] += no;
		read_text (word1);
		kill_chan_with_pri (14);
		put_text ();
		return 0;
	}
	if (!strcmp (word1, "TYPE")) {
		if (check_user ()) return 1;
		if (!strlen (word2)) {
			put_line ("TYPE MUST BE FOLLOWED BY");
			put_line (afwa);
			return 0;
		}
		if (fatal) {
			if (!path && !strcmp (word2, "HYGORORDER")) {
				do_sound (17);
				put_line (pwait1);
				put_line (pwait2);
				put_line ("");
				read_text ("DISK01:TXT/MsgB");
				kill_chan_with_pri (14);
				put_text ();
			} else put_line ("SYSTEM FAILURE !");
			wait_loop (100);
			return 0;
		}
		if (!path && message && !strcmp (word2, "MESSAGE")) {
			do_sound (17);
			put_line (pwait1);
			put_line (pwait2);
			put_line ("");
			strcpy (word1, "DISK01:TXT/Msg0");
			word1 [14] += (message-1);
			read_text (word1);
			kill_chan_with_pri (14);
			put_text ();
			message = 0;
			set_status_col ();
			return 0;
		}
		if (!path && !strcmp (word2, "AKTTASK")) {
			do_sound (17);
			put_line (pwait1);
			put_line (pwait2);
			put_line ("");
			strcpy (word1, "DISK01:TXT/MsgZ");
			if (tacdes) word1 [14] = 'Y';
			if (steexc) word1 [14] = 'X';
			if (ekiman) word1 [14] = 'W';
			if (novbom) word1 [14] = 'V';
			read_text (word1);
			kill_chan_with_pri (14);
			put_text ();
			set_status_col ();
			return 0;
		}
		for (i = 0; i < 8; i++) {
			if (path) strcpy (word1, &(diskfiles [path-1].file [i][1]));
			else strcpy (word1, &(compfiles [comp [no].files].file [i][1]));
			ii = strlen (word2);
			if (ii > 0 && !strncmp (word1, word2, ii)) {
				if (!strncmp (word1+ii, ".TXT", 4)) {
					if (!path) {
						if (compfiles [comp [no].files].file [i][0] == '.') {
							do_sound (0);
							put_line (profile);
							put_line (autcodefil [level]);
							if (!master && strcmp (passfile [level], (char *) get_line ())) {
								do_sound (12);
								intal = 1;
								put_line ("INTRUDER ALERT CLASS I+");
								wait_loop (100);
								return 1;
							}
						}
					}
					if (path) {
						strcpy (word1, "DISK01:TXT/Dsk00");
						word1 [14] += path-1;
						word1 [15] += i;
					} else {
						strcpy (word1, "DISK01:TXT/Txt0A0");
						word1 [14] += level;
						word1 [15] += no;
						word1 [16] += i;
					}
					do_sound (17);
					put_line (pwait1);
					put_line (pwait2);
					put_line ("");
					read_text (word1);
					kill_chan_with_pri (14);
					put_text ();
				} else put_line ("FILE TYPE ERROR+");
				break;
			}
		}
		if (i == 8) put_line ("FILE NOT FOUND ERROR+");
		return 0;
	}
	if (!strcmp (word1, "CD")) {
		if (check_user ()) return 1;
		change_dir (1);
		return 0;
	}
	if (!strcmp (word1, "SHOW")) {
		if (check_user ()) return 1;
		if (!strlen (word2)) {
			put_line ("SHOW MUST BE FOLLOWED BY");
			put_line (afwa);
			return 0;
		}
		if (fatal) {
			put_line ("SYSTEM FAILURE !+");
			wait_loop (50);
			return 0;
		}
		for (i = 0; i < 8; i++) {
			if (path) strcpy (word1, &(diskfiles [path-1].file [i][1]));
			else strcpy (word1, &(compfiles [comp [no].files].file [i][1]));
			ii = strlen (word2);
			if (ii > 0 && !strncmp (word1, word2, ii)) {
				if (!strncmp (word1+ii, ".PIC", 4)) {
					if (!path) {
						if (compfiles [comp [no].files].file [i][0] == '.') {
							do_sound (0);
							put_line (profile);
							put_line (autcodefil [level]);
							if (!master && strcmp (passfile [level], (char *) get_line ())) {
								do_sound (12);
								intal = 1;
								put_line ("INTRUDER ALERT CLASS I+");
								wait_loop (100);
								return 1;
							}
						}
					}
					if (path) {
						strcpy (word1, "DISK01:TXT/Pic00");
						word1 [14] += path-1;
						word1 [15] += i;
					} else {
						strcpy (word1, "DISK01:TXT/Pic0A0");
						word1 [14] += level;
						word1 [15] += no;
						word1 [16] += i;
					}
					do_sound (17);
					clr_comp ();
					load_riss (word1);
					kill_chan_with_pri (14);
					key ();
					put_line ("");
				} else put_line ("FILE TYPE ERROR+");
				break;
			}
		}
		if (i == 8) put_line ("FILE NOT FOUND ERROR+");
		return 0;
	}
	if (!strcmp (word1, "INFO")) {
		do_sound (16);
		sprintf (word1, "YOU ARE ON DECK %d", level+1);
		put_line (word1);
		if (level > 6) put_line ("IN SECTION I");
		else if (level > 3) put_line ("IN SECTION II");
		else put_line ("IN SECTION III");
		if (xray) {
			sprintf (word1, "PERSONAL RADIATION  %d%%", xray);
			put_line (word1);
		}
		if (green) put_line ("WARNING! - TOXIC ATMOSPHEAR!");
		put_line ("YOU HAVE");
		sprintf (word1, " %d UNITS OF FUEL", fuel);
		put_line (word1);
		sprintf (word1, " %d MINES", mines);
		put_line (word1);
		strcpy (word1, "DISK ");
		for (i = 0; i < 8; i++) {
			sprintf (word2, "%d ", i+1);
			if (disk & (1<<i)) strcpy (word1+strlen (word1), word2);
		}
		if (strlen (word1) > 5) put_line (word1);
		put_line("+");
		kill_chan_with_pri (13);
		return 0;
	}
	if (!strcmp (word1, "CLS")) {
		clr_comp ();
		put_line ("");
		return 0;
	}
	if (!strcmp (word1, "HIFI")) {
		if (!strlen (word2)) {
			put_line ("ON OR OFF ?+");
			return 0;
		}
		if (!strcmp (word2, "ON")) {
			*led |= 2;
			return 0;
		}
		else if (!strcmp (word2, "OFF")) {
			*led &= 0xFD;
			return 0;
		}
	}
	if (!strcmp (word1, "SETMAP")) {
		if (!strlen (word2)) {
			if (keycode[35] == (char) (0x9D)) put_line ("GERMAN KEYMAP+");
			else put_line ("ENGLISH KEYMAP+");
			return 0;
		}
		if (!strcmp (word2, "D")) {
			keycode[35] = 0x9D;
			keycode[36] = 0xD5;
			put_line ("GERMAN KEYMAP+");
		} else if (!strcmp (word2, "USA")) {
			keycode[35] = 0xD5;
			keycode[36] = 0x9D;
			put_line ("ENGLISH KEYMAP+");
		}
		return 0;
	}
	if (!strcmp (word1, "DERUR") && path == 4) {
		if (intal >= 10) {
			put_line ("INTRUDER ALERT CLASS II");
			put_line ("");
			for (i = 0; i < 30000; i++);
			put_line ("NASKUR-DETONATOR IGNITION");
			put_line ("SEQUENCE HAS ALREADY");
			put_line ("STARTED");
			put_line ("+");
			wait_loop (100);
			return 1;
		} else if (intal) {
			kill_chan_with_pri (12);
			put_line ("ALERT CANCELED+");
			intal = 0;
		} else put_line ("NO ALERT TO CANCEL+");
		return 0;
		strcmp (gtime, get_line ());
	}
	if (fatal) {
		put_line ("NO ORDERS ACCEPTED !+");
		return 1;
	}
	if (!strcmp (word1, "CLIMATE") && path == 8) {
/*		enigma6 ();*/
		if (check_user ()) return 1;
		if ((level == 5 && no == 4) || master) {
			if( !strcmp( word2, "DECK" ) && zahl >= 1 && zahl <= 8 ) {
				do_sound (0);
				do_air (zahl-1);
				do_sound (9);
			} else put_line( "INVALID PARAMETERS+" );
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
	}
	if (!strcmp (word1, "MOEBIUS") && path == 4) {
		if (check_user ()) return 1;
		moebius ();
		if ((level == 6 && no == 8) || master) {
			do_sound (8);
			put_line ("ENERK SUPERVISING");
			put_line ("SYSTEM CORRUPTED !");
			put_line ("");
			put_line ("ALL ENERKS DISABLED+");
			gorg = 1;
			tarpen = 1;
			for (i = 0; i < foes; i++)
				if (foef [i] == 2) foed [i] = 0;
		}
		if (level == 7 && no == 9) {
			put_fatal ();
			fatal = 4;
		}
		return 0;
	}
	if (!strcmp (word1, "DERURTRN") && path == 3) {
		if (check_user ()) return 1;
		if ((level == 7 && no == 4) || master) {
			put_fatal ();
			fatal = 1;
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
	}
	if (!strcmp (word1, "MGARRYN") && path == 0 && level == 7 && (no == 11 || master)) {
		if (check_user ()) return 1;
		if (!strcmp (word2, "TO") && zahl >= 1 && zahl <= 7) {
			do_sound (9);
			sprintf (word1, "MULTI-GARRYN SET TO DECK %d+", zahl);
			exitpos [0].a = zahl - 8;
			put_line (word1);
		} else if (!strcmp (word2, "LIST")) {
			sprintf (word1, "MULTI-GARRYN SET TO DECK %d+", 8 + exitpos [0].a);
			put_line (word1);
		} else put_line ("LOOKUP FORMAT IN HELPFILE+");
		return 0;
		strcmp (&score, get_line ());
	}
	if (!strcmp (word1, "VERS")) {
		sprintf (word1, "VERSION NUMMER: %d", versno);
		put_line (word1);
		return 0;
	}
	if (!strcmp (word1, "POSITION") && path == 0 && level == 7 && (no == 10 || master)) {
		if (check_user ()) return 1;
		do_sound (0);
		if (!steexc) sprintf (word1, "%ld BIL KM AWAY FROM TARGET PLANET", 10000-itime);
		if (tarpen) sprintf (word1, "%ld MIL KM AWAY FROM IGNITION POINT", 5000-itime);
		if (novbom) strcpy (word1, "xyz KM AWAY FROM SOL");
		put_line (word1);
		return 0;
	}
	if (!strcmp (word1, "ORDER298") && path == 7) {
		if (check_user ()) return 1;
		if ((level == 2 && no == 8) || master) {
/*			enigma3 ();*/
			if (hygkoo) {
				do_sound (0);
				put_line ("ORDER CAN NOT BE");
				put_line ("EXECUTED WITHOUT");
				put_line ("HYGOR SUPPORT+");
			} else {
				do_sound (9);
				put_line ("ORDER RECEIVED !");
				put_line ("");
				put_line ("THE SEMORCOM IS NO LONGER");
				put_line ("EMITING SEXADIMRAYS AND");
				put_line ("OUT OF ORDER+");
				semcom = 1;
			}
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
	}
	if (!strcmp (word1, "ORDER096") && path == 7) {
		if (check_user ()) return 1;
		if ((level == 2 && no == 8) || master) {
			do_sound (0);
			if (semcom || hygkoo) {
				put_line ("SEMORCOM");
				put_line ("IS OUT OF");
				put_line ("ORDER+");
			} else {
				put_line ("ORDER RECEIVED !");
				put_line ("");
				put_line ("A MEANINGLESS MESSAGE");
				put_line ("HAS BEEN SENT+");
				semcom = 1;
			}
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
	}
	if (!strcmp (word1, "CORURDEIMAR") && path == 6) {
		if (check_user ()) return 1;
		if ((level == 7 && no == 6) || master) {
			do_sound (0);
			put_line ("DEIMARdcafb SETUP TOOL");
			put_line ("WRITTEN BY abcdefgh");
			put_line ("");
			put_line ("ENTER MASS OF OBJECT");
			mass = atoi ((char *) get_line ());
			put_line ("ENTER DESTINATION ab");
			dka = atoi ((char *) get_line ());
			put_line ("ENTER DESTINATION cd");
			dkb = atoi ((char *) get_line ());
			put_line ("ENTER DESTINATION ef");
			dkc = atoi ((char *) get_line ());
			put_line ("ENTER TIME OFFSET");
			ttime = (atol ((char *) get_line ()) + score);
			do_sound (9);
			put_line ("NEW SETUP ACCEPTED !");
			put_line ("THE DEIMAR SYSTEM WILL BE");
			sprintf (word1, "ACTIVATED AT MISSIONTIME %ld+", ttime);
			put_line (word1);
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
		strcmp (&master, get_line ());
	}
	if (!strcmp (word1, "DERURVIHF") && path == 3) {
		if (check_user ()) return 1;
		if ((level == 1 && no == 10) || master) {
			put_fatal ();
			fatal = 2;
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
	}
	if (!strcmp (word1, "DERURTACHYN") && path == 1) {
		if (check_user ()) return 1;
		if ((level == 2 && no == 7) || master) {
			if (tacdes == 1) {
				put_line ("NO EFFECT+");
				return 0;
			}
			do_sound (7);
			put_line ("TACHION-DESOLATOR IS DISABLED !");
			put_line ("");
			wait_loop (50);
			put_line ("A ENERGY PLATFORM ON DECK 7-1.F");
			put_line ("HAS BEEN DISABLED");
			put_line ("");
			do_sound (14);
			put_line (new1);
			put_line (new2);
			if (itime < 2000) itime = 2000;
			tacdes = 1;
			wait_loop (100);
			do_sound (0);
			put_line ("");
			put_line ("A PRIORORITY ORDER DECREMENTED");
			put_line ("THE IGNITION TIMER !");
			put_line ("USE STATUS TO CHECK.");
			put_line ("+");
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
	}
	if (!strcmp (word1, "DERURASSIM") && path == 2) {
		if (check_user ()) return 1;
		if ((level == 6 && no == 10) || master) {
			if (steexc == 1) {
				put_line ("NO EFFECT+");
				return 0;
			}
/*			enigma7 ();*/
			do_sound (8);
			put_line ("SOLAR-ASSIMILATOR IS NOW DISABLED");
			put_line ("");
			wait_loop (50);
			put_line ("A GATE ON DECK 7-1.D");
			put_line ("HAS BEEN DISABLED");
			put_line ("");
			dev_off (20);
			do_sound (14);
			put_line (new1);
			put_line (new2);
			if (itime < 8000) itime = 8000;
			steexc = 1;
			wait_loop (100);
			do_sound (0);
			put_line ("VARGHUR STARTS MOVING");
			put_line ("AWAY FROM SOL !");
			wait_loop (100);
			do_sound (0);
			put_line ("");
			put_line ("A PRIORORITY ORDER DECREMENTED");
			put_line ("THE IGNITION TIMER !");
			put_line ("USE STATUS TO CHECK.");
			put_line ("+");
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
	}
	if (!strcmp (word1, "DERUREKIN") && path == 0 && ((level == 3 && no == 2) || master)) {
		if (check_user ()) return 1;
		if (steexc) {
			if (ekiman == 1) {
				put_line ("NO EFFECT+");
				return 0;
			}
			put_line ("EKIN-MANIPULATOR IS STOPPED");
			do_sound (8);
			do_sound (7);
			put_line ("");
			wait_loop (50);
			put_line (new1);
			put_line (new2);
			ekiman = 1;
			steexc = 0;
			if (itime < 9500) itime = 9500;
			wait_loop (100);
			do_sound (0);
			put_line ("");
			put_line ("A PRIORORITY ORDER INCREMENTED");
			put_line ("THE IGNITION TIMER !");
			put_line ("USE STATUS TO CHECK.");
			put_line ("+");
		} else {
			put_line ("YOU CAN NOT STOP AN");
			put_line ("NON ACTIVE DEVICE !+");
		}
		return 0;
	}
	if (!strcmp (word1, "DERURNSKR") && path == 5) {
/*		enigma5 ();*/
		if (check_user ()) return 1;
		if ((level == 4 && no == 5) || master) {
			if (hygkoo) {
				if (seldes == 1) {
					put_line ("NO EFFECT+");
					return 0;
				}
				novbom = itime = 0;
				do_sound (8);
				do_sound (7);
				put_line ("NASKUR-DETONATOR IGNITION");
				put_line ("SEQUENCE ABORTED+");
				seldes = 1;
				put_line ("");
				wait_loop (50);
				put_line (new1);
				put_line (new2);
				wait_loop (100);
				do_sound (2);
				put_line ("");
				put_line ("SELFDESTRUCTION ACTIVATED !");
				put_line ("+");
				return 0;
			} else {
				put_fatal ();
				fatal = 3;
				if (level == 4) {
					dev_on (14);
					dev_on (15);
					dev_on (16);
					dev_on (17);
				}
				do_sound (13);
				put_line ("PASSAGE CLOSED !+");
			}
		} else {
			put_line (noab);
			put_line (toru);
		}
		return 0;
	}
	if (!strcmp (word1, "ENDCLI")) {
/*		if (level == 7 && no == 8) enigma8 ();*/
		return 1;
	}
	if (strlen (word1)) {
		if (path) put_line ("FILE NOT FOUND ERROR+");
		else put_line ("INVALID INPUT+");
	}
	return 0;
}

change_dir (mode)
int mode;
{
	if (master) {
		if (!strcmp (word2, "SYSTEM")) path = 0;
		else if (!strcmp (word2, "DISK") && zahl >= 1 && zahl <= 8) path = zahl;
	} else {
		if (!strcmp (word2, "SYSTEM")) path = 0;
		else if (!strcmp (word2, "DISK") && (disk & (1 << (zahl - 1)))) path = zahl;
		else if (strlen (word2)) {
			put_line ("DEMANDED DISK IS NOT AVAILABLE");
			put_line ("CHANGE TO SYSTEM FOR MORE INFORMATION+");
			return 1;
		}
	}
	if (!mode) return 0;
/*	enigma1 ();*/
	put_line ("CURRENT PATH");
	if (!path) {
		put_line (" SYSTEM+");
	} else {
		strcpy (word2, "DISK 0+");
		word2[5] += path;
		put_line (word2);
	}
}
