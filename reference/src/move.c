#define SMAPP    21

extern struct StartDef {
	int x [2], y [2];
} startdef [MAXLEVELS];

extern struct ExitDef {
	int x, y, a;
} exitpos [6];

extern UWORD matrix [15][128];

extern WORD statr, statg, statb, esc;
extern WORD dark, button, joyx, joyy, akbob, level, disk, leveladd;
extern WORD fueladd, master, sinnx, sinny, collision, fuel, mines;
extern WORD clrlistdy, ftime;
extern UWORD bobx, boby, *bobimage, bobon, xpos;
extern ULONG spos;
extern LONG itime, score;

extern UWORD *girlgr [16], *girlgl [16];
extern UWORD *girlar [4], *girlal [4];
extern UWORD *girlgsr [8], *girlgsl [8];
extern UWORD *girlasr [8], *girlasl [8];
extern UWORD *girld [4], *girle [21], *girltr [8];

WORD exno, down, hide, countdown, minex, miney;
WORD flying, girlshp, specmv, tthrust, deltax;
WORD xp, yp, vely, velx, myxd, middle;
LONG mxp;

init_move (level)
int level;
{
	set_status_col ();
	spos = (xpos = startdef [level].x [exno]) * 32L;
	mxp = (xpos + 130) * 32L;
	xp = 130 * 32;
	yp = startdef [level].y [exno];
	down = hide = middle = countdown = myxd = velx = vely = flying = 0;

	girlshp = 82;
	specmv = SMAPP;

	make_display ();

	move_own ();
	while (akbob);
	DrawBobs ();
	ScrollBG ();
	akbob = -1;

	color_light (dark);
	if (ftime) {
		light_puls ();
		ftime = 0;
	}
	do_sound (1);
	bobon = 1;
}

move_own ()
{
	no_name ();
	anim_joystick ();
	check_walls ();
	get_new_anim ();
	grav_sim ();
	check_ground ();
	add_x ();
	add_check_y ();
	scroll ();
	set_bob ();
}

anim_joystick ()
{
	switch (specmv) {
		case SMGROUND:	;
		case SMFLY:		joystick ();
							break;
		case SMSTARTR:	joyx = 1;
							goto contstartground;
		case SMSTARTL:	joyx = -1;
contstartground:		joyy = 0;
							if ((++girlshp) > 15) {
								joystick ();
								specmv = SMGROUND;
								girlshp = 0;
							}
							break;
		case SMSAIRR:	joystick ();
							joyx = 1;
							goto contstartair;
		case SMSAIRL:	joystick ();
							joyx = -1;
contstartair:			if ((++girlshp) > 15) {
								joystick ();
								specmv = SMFLY;
								girlshp = 0;
							}
							break;
		case SMSTOPR:	;
		case SMSTOPL:	joyx = joyy = 0;
							if ((--girlshp) < 0) {
								girlshp = 0;
								if (!velx) {
									joystick ();
									specmv = SMGROUND;
								}
							}
							break;
		case SMEAIRR:	;
		case SMEAIRL:	joystick ();
							joyx = 0;
							if ((--girlshp) < 0) {
								girlshp = 0;
								if (!velx) {
									joystick ();
									specmv = SMFLY;
								}
							}
							break;
		case SMHIDEB:	hide = 1;
							if ((++girlshp) > 15) {
								girlshp = 15;
								joystick ();
								if (joyy != -1) specmv = SMHIDEE;
								joyy = joyx = 0;
							} else {
								joyy = joyx = 0;
								yp -= 32;
								xp += 32;
								mxp += 32;
							}
							break;
		case SMHIDEE:	hide = 1;
							if ((--girlshp) < 0) {
								girlshp = 0;
								joystick ();
								hide = 0;
								specmv = SMGROUND;
							} else {
								joyy = joyx = 0;
								yp += 32;
								xp -= 32;
								mxp -= 32;
							}
							break;
		case SMTURNE:	joyx = joyy = 0;
							if ((--girlshp) < 0) {
								joystick ();
								specmv = SMGROUND;
								girlshp = 0;
							}
							break;
		case SMTURNS:	joyx = joyy = 0;
							if ((++girlshp) > 31) {
								girlshp = 31;
								if (!middle) break;
								use_dev ();
								specmv = SMTURNE;
								joystick ();
							}
							break;
		case SMUP:		down = 1;
							joyx = joyy = 0;
							if ((--girlshp) < 0) {
								joystick ();
								specmv = SMGROUND;
								girlshp = down = 0;
							}
							break;
		case SMDOWN:	down = 1;
							joystick ();
							if ((++girlshp) > 15) {
								girlshp = 15;
								if (joyy != 1) specmv = SMUP;
							}
							joyx = joyy = 0;
							break;
		case SMTAKEU:	joyx = joyy = 0;
							if ((--girlshp) < 0) {
								joystick ();
								specmv = SMGROUND;
								girlshp = 0;
							}
							break;
		case SMTAKED:	joyx = joyy = 0;
							if ((++girlshp) > 15) {
								girlshp = 15;
								specmv = SMTAKEU;
							}
							break;
		case SMLIFTUP:	joyx = 0;
							if (tthrust == LIFTLIFE) {
								joyy = 0;
								if (deltax > 0) {
									xp += 16;
									mxp += 16;
									--deltax;
								} else if (deltax < 0) {
									xp -= 16;
									mxp -= 16;
									++deltax;
								}
								if (girlshp < 31) ++girlshp;
								else if (!deltax) --tthrust;
							} else if (!tthrust) {
								joyy = 0;
								if (!vely) {
									if (girlshp > 0) --girlshp;
									else {
										joystick ();
										clrlistdy -= 5;
										remake_ground (MAGRAV | MGROUND, sinnx, sinny);
										remake_ground ((MAGRAV + 1) | MGROUND, sinnx+1, sinny);
										remake_ground ((MAGRAV + 2) | MGROUND, sinnx+2, sinny);
										specmv = SMGROUND;
									}
								}
							} else {
								--tthrust;
								joyy = -1;
							}
							break;
		case SMLIFTDN:	joyx = 0;
							if (tthrust == LIFTLIFE) {
								joyy = 0;
								if (deltax > 0) {
									xp += 16;
									mxp += 16;
									--deltax;
								} else if (deltax < 0) {
									xp -= 16;
									mxp -= 16;
									++deltax;
								}
								if (girlshp < 31) ++girlshp;
								else if (!deltax) tthrust -= 2;
							} else if (!tthrust) {
								if (!vely) {
									joyy = 0;
									if (girlshp > 0) --girlshp;
									else {
										while (((yp>>6)&15) != 11) --yp;
										joystick ();
										remake_ground (MAGRAV | MGROUND, sinnx, sinny);
										remake_ground ((MAGRAV + 1) | MGROUND, sinnx+1, sinny);
										remake_ground ((MAGRAV + 2) | MGROUND, sinnx+2, sinny);
										specmv = SMGROUND;
									}
								} else joyy = -1;
							} else {
								--tthrust;
								joyy = 1;
							}
							break;
		case SMAPP:	if ((--girlshp) < 0) {
							joystick ();
							specmv = SMGROUND;
							girlshp = 0;
						} else joyx = joyy = 0;
						break;
	}
}

get_new_anim ()
{
register int x, y, var;

	y = yp >> 10;
	x = mxp >> 9;

	if (!velx && collision == 1 && !master) {
		do_sound (1);
		girlshp = 0;
		specmv = SMDIE;
		collision = 2;
	}

	switch (specmv) {
		case SMFLY:		if (!velx) {
								if (joyx == 1) {
									specmv = SMSAIRR;
									girlshp = 0;
								} else if (joyx == -1) {
									specmv = SMSAIRL;
									girlshp = 0;
								}
							} else if (velx >= 15 && joyx != 1) {
								specmv = SMEAIRR;
								girlshp = 15;
								break;
							} else if (velx <= -15 && joyx != -1) {
								specmv = SMEAIRL;
								girlshp = 15;
								break;
							}
							break;
		case SMGROUND:	if (button && !velx) {
								if (!sinny && ((var = (matrix [y+3][x+1] & 0x01FF)) == MAGRAV) || (var == MAGRAV+1) || (var == MAGRAV+2)) {

									for (var = 0; var < 6; var++)
										if (joyy == -1 && x == exitpos [var].x && y == exitpos [var].y) {
											leveladd = exitpos [var].a;
											if (leveladd > 0) exno = 0;
											else exno = 1;
											break;
										}

									if (joyy == -1 && (((var = (matrix [y-1][x+1] & 0x01FF)) == MAGRAV) || (var == MAGRAV+1) || (var == MAGRAV+2))) {
										if ((matrix [y+3][x+1] & 0x1FF) == MAGRAV) {
											deltax = 2*(15 - ((mxp >> 5) & 15))+6;
											sinnx = x+1;
										} else {
											deltax = -2*((mxp >> 5) & 15)+6;
											sinnx = x;
											if (var == MAGRAV+2) {
												deltax -= 32;
												sinnx = x-1;
											}
										}
										specmv = SMLIFTUP;
										sinny = y-1;
										girlshp = 0;
										remake_ground ((MAGRAV + 6) | MGROUND, sinnx, sinny);
										remake_ground ((MAGRAV + 7) | MGROUND, sinnx+1, sinny);
										remake_ground ((MAGRAV + 8) | MGROUND, sinnx+2, sinny);
										tthrust = LIFTLIFE;
										do_sound (5);   /* lift */
										break;
									} else if (joyy == 1 && (((var = (matrix [y+7][x+1] & 0x01FF)) == MAGRAV) || (var == MAGRAV+1) || (var == MAGRAV+2))) {
										if ((matrix [y+3][x+1] & 0x1FF) == MAGRAV) {
											deltax = 2*(15 - ((mxp >> 5) & 15))+6;
											sinnx = x+1;
										} else {
											deltax = -2*((mxp >> 5) & 15)+6;
											sinnx = x;
											if (var == MAGRAV+2) {
												deltax -= 32;
												sinnx = x-1;
											}
										}
										specmv = SMLIFTDN;
										sinny = y+3;
										clrlistdy -= 5;
										girlshp = 0;
										remake_ground( matrix [sinny][sinnx-1], sinnx-1, sinny);
										remake_ground( matrix [sinny-1][sinnx-1], sinnx-1, sinny-1);
										remake_ground( matrix [sinny-1][sinnx], sinnx, sinny-1);
										remake_ground( matrix [sinny-1][sinnx+1], sinnx+1, sinny-1);
										remake_ground( matrix [sinny-1][sinnx+2], sinnx+2, sinny-1);
										remake_ground( MAGRAV + 6, sinnx, sinny);
										remake_ground( MAGRAV + 7, sinnx+1, sinny);
										remake_ground( MAGRAV + 8, sinnx+2, sinny);
										tthrust = LIFTLIFE;
										do_sound (5);   /* lift */
										break;
									}
								}
								if (joyy == -1) {
									if (((matrix [y][x+1] & 0x01FE) == MCOMPUTER) || ((matrix [y][x+1] & 0x01FE) == MSWITCH)) {
										joyy = vely = girlshp = 0;
										specmv = SMTURNS;
										break;
									} else if ((matrix [y][x+1] & 0x01FE) == MHIDE) {
										joyy = vely = girlshp = 0;
										specmv = SMHIDEB;
										break;
									}
								}
								if (!sinny && joyy == 1) {
									if (matrix [y+3][x+1] & CHECKTHING) {
										girlshp = 0;
										specmv = SMTAKED;
										if (countdown && minex == x+1 && miney == y+3)
											countdown = 0;
										if ((matrix [y+3][x+1] & CHECKTHING) == MMINE) {
											if (mines < 9) ++mines;
											do_sound (0);
											new_mines ();
										} else if ((matrix [y+3][x+1] & CHECKTHING) == MFUEL) {
											do_sound (0);
											fueladd += 200;
										} else if ((matrix [y+3][x+1] & CHECKTHING) == MDISK1) {
											do_sound (0);
											disk |= (1 << level);
										}
										clr_obj (x+1, y+3);
										break;
									} else if (mines && !countdown && !(matrix [y+3][x+1] & CHECKTHING) && (matrix [y+3][x+1] & 0x01ff) >= FRGREL && (matrix [y+3][x+1] & 0x01ff) <= LAGREL) {
										girlshp = 0;
										specmv = SMTAKED;
										do_sound (0);
										countdown = 300;
										minex = x+1;
										miney = y+3;
										set_obj (MMINE, minex, miney);
										--mines;
										new_mines ();
										break;
									}
									girlshp = 0;
									specmv = SMTAKED;
								}
								joyy = 0;
							}
							if (!velx) {
								if (joyx == 1) {
									specmv = SMSTARTR;
									girlshp = 0;
									break;
								} else if (joyx == -1) {
									specmv = SMSTARTL;
									girlshp = 0;
									break;
								}
								if (!vely && joyy == 1) {
									specmv = SMDOWN;
									girlshp = 0;
									break;
								}
							} else if (velx >= 15 && joyx != 1) {
								specmv = SMSTOPR;
								girlshp = 15;
								break;
							} else if (velx <= -15 && joyx != -1) {
								specmv = SMSTOPL;
								girlshp = 15;
								break;
							}
	}
}

scroll ()
{
register int scroll, x;

	scroll = 0;
	if (xp < 80*32 && velx < 0 || xp > 180*32 && velx > 0) scroll = 1;
	if (xpos >= 1728 && velx > 0 || xpos <= 1 && velx < 0) scroll = 0;

	mxp += velx;
	if (scroll) spos += velx;
	else xp += velx;
	middle = 0;
	if (!velx && !joyx) {
		if (xp > 131*32 && xpos < 1728) {
			spos += 32;
			xp -= 32;
		} else if (xp < 129*32 && xpos > 1) {
			spos -= 32;
			xp += 32;
		} else middle = 1;
	}
	if ((x = (spos>>5)) > xpos) myxd = 1;
	else if (x < xpos) myxd = -1;
	else myxd = 0;
}

set_bob ()
{
	bobx = xp>>5;
	boby = yp>>6;

	switch (specmv) {
			case SMFLY:		if (velx > 0) bobimage = girlar [girlshp>>2];
								else if (velx < 0) bobimage = girlal [girlshp>>2];
								else bobimage = girlasr [0];
								++girlshp;
								girlshp &= 15;
								break;
			case SMGROUND:	if (velx > 0) {
									bobimage = girlgr [girlshp>>2];
									if (girlshp == 31 || girlshp == 63) do_sound (20);
								} else if (velx < 0) {
									bobimage = girlgl [girlshp>>2];
									if (girlshp == 31 || girlshp == 63) do_sound (20);
								} else bobimage = girlgsr [0];
								++girlshp;
								girlshp &= 63;
								break;
			case SMSTOPR:	;
			case SMSTARTR:	bobimage = girlgsr [girlshp>>1];
								break;
			case SMSTOPL:	;
			case SMSTARTL:	bobimage = girlgsl [girlshp>>1];
								break;
			case SMEAIRR:	;
			case SMSAIRR:	bobimage = girlasr [girlshp>>1];
								break;
			case SMEAIRL:	;
			case SMSAIRL:	bobimage = girlasl [girlshp>>1];
								break;
			case SMTURNE:	;
			case SMTURNS:	;
			case SMHIDEB:	;
			case SMHIDEE:	bobimage = girltr [(girlshp>>2)];
								break;
			case SMUP:		;
			case SMDOWN:	;
			case SMTAKEU:	;
			case SMTAKED:	bobimage = girld [(girlshp>>2)];
								break;
			case SMLIFTUP:	bobimage = girltr [(girlshp>>2)];
								if (tthrust < LIFTLIFE) {
									bobimage = girltr [7];
									if (!vely) bobimage = girltr [(girlshp>>2)];
								}
								break;
			case SMLIFTDN:	bobimage = girltr [(girlshp>>2)];
								if (tthrust < LIFTLIFE) {
									bobimage = girltr [7];
									if (!vely) bobimage = girltr [(girlshp>>2)];
								}
								break;
			case SMDIE:		bobimage = girle [(girlshp>>2)];
								++girlshp;
								if (girlshp > 81) {
									girlshp = 81;
									bobon = 0;
								}
								break;
			case SMAPP:		bobimage = girle [(girlshp>>2)];
	}
}

grav_sim ()
{
static thrust = 0;

	if (joyy < 0 && (fuel || specmv == SMLIFTDN || specmv == SMLIFTUP)) {
		--vely;
		if (fuel && (specmv != SMLIFTDN && specmv != SMLIFTUP)) {
			if (!thrust) {
				do_sound (3);
				thrust = 1;
			}
			if (!master) --fuel;
			disp_fuel ();
		}
	} else {
		if (thrust) {
			kill_chan_with_pri (15);
			thrust = 0;
		}
		if (vely < 64)
			vely += (specmv != SMAPP && specmv != SMLIFTDN && specmv != SMHIDEB && specmv != SMHIDEE)? 1: joyy;
	}

	if (vely > 64) vely = 64;
	else if (vely < -64) vely = -64;
}

add_x ()
{
	velx += joyx;
	if (!joyx) {
		if (velx > 0) velx -= 1;
		else if (velx < 0) velx += 1;
	} else {
		if (velx > 0 && joyx < 0) velx -= 1;
		else if (velx < 0 && joyx > 0) velx += 1;
	}
	if (velx < -32) velx = -32;
	else if (velx > 32) velx = 32;
}

add_check_y ()
{
	yp += vely;
	if (yp < 0) {
		yp = 0;
		vely = 0;
	}
	if (yp > 180*64) {
		yp = 180*64;
		vely = 0;
		if (level == 7) {
			leveladd = -4;
			exno = 0;
			return;
			enigma5 ();
		}
		if ((!master && !collision) || esc) {
			collision = 2;
			bobon = 0;
		}
	}
}
