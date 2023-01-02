/* census.c - census routines */

# include "ve.h"

/*
 * Input file processing functions
 *
 * These routines parse the information from the input files,
 * many times in a very ugly way.
 *
 * Each routine checks to see if there is a comma in the 4th position.
 * This is to make certain that the line is of the form x,y and not
 * another message (e.g. Bad weather).
 */

/*
 * census - Read and process census file.
 */
census(fp)
	FILE   *fp;
{
	register int x, y;
	register char *p;
	Sector *mp;
	Value  *vp;
	char    buf[BUFSIZ];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[4] != ',') {
			/*
			 * Look for census lines of the
			 * form: 'xxxxx yyy zzz in x,y'
			 * (reports of products produced)
			 */
			if (p = rindex(buf, ',')) {
				if (*++p == '-')
					p++;
				if (noise && *p >= '0' && *p <= '9') {
					fputs(buf, stdout);
					(void) fflush(stdout);
				}
			}
			continue;
		}
		x = xoffset(atoi(buf));
		y = yoffset(atoi(&buf[5]));
		maxxy(x, y);
		mp = map[x][y];
		if (mp == NULL)
			map[x][y] = mp = newmap();
		if (mp->vp == NULL)
			mp->vp = newval();
		vp = mp->vp;
		mp->own = 1;
		vp->del[CIV] = -1;
		vp->del[MIL] = -1;
		vp->del[UW] = buf[22];
		vp->del[FOOD] = buf[23];
		vp->dist[UW] = buf[25];
		vp->dist[FOOD] = buf[26];
		vp->val[CHKPT] = buf[28];
		vp->val[COU] = -1;
		vp->val[DES] = merge(buf[10], x, y, TRUE);
		vp->val[SDES] = buf[11];
		if (isupper(buf[11])) {
			mp->own = 2;
			vp->val[COU] = buf[11] - 'A';
			vp->val[SDES] = ' ';
			mp->sdes = ' ';
		} else {
			mp->sdes = (buf[11] == '-') ?
			    '~' : ((buf[11] == '^') ? '&' : buf[11]);
		}
		vp->val[EFF] = atoi(&buf[13]);
		vp->val[MOB] = atoi(&buf[18]);
		sscanf(&buf[30], "%hd%hd%hd%hd",
		       &vp->val[CIV], &vp->val[MIL], &vp->val[UW],
		       &vp->val[FOOD]);
		vp->val[WORK] = atoi(&buf[50]);
		vp->val[AVAIL] = atoi(&buf[56]);
		if (buf[61])
			vp->val[TERR] = atoi(&buf[61]);
		else
			vp->val[TERR] = 0;
	}
}
/*
 * censusinfo - Display census and commodities info for sector.
 */
censusinfo(x, y)
	register int x, y;
{
	register Sector *mp;
	register int nx, ny;
	register Unit *up;
	register Ship *sp;
	Value  *vp;
	Level  *lp;
	register struct planeentry *pp;
	int     censusmode = 0, lmode = 0;
	char   *getnam();

	nx = xoffset(x);
	ny = yoffset(y);
	mp = map[nx][ny];
	censusmode = (shipmode && mp && mp->shp != NOSHIPS) +
	    2 * (planemode && mp && mp->pln != NOPLANES) +
	    4 * (unitmode && mp && mp->unt != NOUNITS);
	switch (levelmode) {
	case 0:
		lmode = 0;
		break;
	case 1:
	case 2:
	case 4:
		if (!mp || (mp->lp != NULL))
			lmode = levelmode;
		break;
	}
	censusheader(censusmode, lmode);

	move(LINES - 2, 0);
	clrtoeol();
	move(LINES - 4, 0);
	clrtoeol();
	if (!mp || !(mp->own)) {
		printw("%4d,%-4d", x, y);
		return;
	}

	if (mp->own) {
		if ((!shipmode || mp->shp == NOSHIPS)
		 && (!unitmode || mp->unt == NOUNITS)) {
			printw("%3d,%-4d%c%c", x, y, mp->des ? mp->des : ' ',
			       mp->sdes ? mp->sdes : ' ');
			if ((vp = mp->vp) != NULL)
				printw("%4d%%%4d %c%c %c%c %c%5d%5d%5d%5d%4d%5d%5d%4d%5d%4d%%%3d",
				       vp->val[EFF], vp->val[MOB],
				       vp->del[UW], vp->del[FOOD],
				       vp->dist[UW], vp->dist[FOOD],
				       vp->val[CHKPT], vp->val[CIV],
				       vp->val[MIL], vp->val[UW],
				       vp->val[FOOD], vp->val[MIN],
				       vp->val[GOLD], vp->val[FERT],
				       vp->val[OIL], vp->val[URAN],
				       vp->val[WORK], vp->val[TERR]);
		} else if (shipmode){
			sp = ships[mp->shp];
			printw("%4d  %-16.16s %3d,%-3d  %c  ",
			       sp->number, sp->type, x, y, sp->fleet);
			if ((vp = sp->vp) == NULL)
				return;
			printw("%5d%%%5d%5d%5d%5d%5d%5d%6d", vp->val[EFF],
			       vp->val[CIV], vp->val[MIL], vp->val[UW],
			       vp->val[FOOD], vp->val[PL], vp->val[MOB],
			       vp->val[FUEL]);
		} else if (unitmode){
			up = units[mp->unt];
			printw("%4d  %-16.16s %3d,%-3d  %c  ",
			       up->number, up->type, x, y, up->army);
			if ((vp = up->vp) == NULL)
				return;
			printw("%5d%%%5d%5d%5d%5d%5d%5d%%%5d%5d", vp->val[EFF],
			       vp->val[FORT],vp->val[MOB],vp->val[FOOD],
			       vp->val[FUEL], vp->val[TECH], vp->val[RETR],
			       vp->val[RADIUS],vp->val[XL]);
		}

		move(LINES - 2, 0);
		if ((!shipmode  || mp->shp == NOSHIPS)  &&
		    (!planemode || mp->pln == NOPLANES) &&
		    (!unitmode  || mp->unt == NOUNITS)) {
			switch (lmode) {
			case 0:
				if (vp == NULL)
					return;
				if (vp->val[COU] == -1)
					addstr("   ");
				else
					printw("%3d", vp->val[COU]);
				if (vp->del[SH])
					printw(" %c%c%c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c%c%c",
					       vp->del[SH], vp->del[GUN],
					       vp->del[PET], vp->del[IRON],
					       vp->del[DUST], vp->del[BAR],
					       vp->del[CRU], vp->del[LCM],
					       vp->del[HCM], vp->del[RAD],
					       vp->dist[SH], vp->dist[GUN],
					       vp->dist[PET], vp->dist[IRON],
					       vp->dist[DUST], vp->dist[BAR],
					       vp->dist[CRU], vp->dist[LCM],
					       vp->dist[HCM], vp->dist[RAD]);
				else
					addstr("                     ");

				printw("%4d%5d%5d%5d%5d%5d%5d%5d%5d%4d%5d",
				       vp->val[SH], vp->val[GUN],
				       vp->val[PET], vp->val[IRON],
				       vp->val[DUST], vp->val[BAR],
				       vp->val[CRU], vp->val[LCM],
				       vp->val[HCM], vp->val[RAD],
					   vp->val[AVAIL]);
				break;
			case 1:
				lp = mp->lp;
				printw("%10.10s%4d,%-3d%4d%5d%4d%4d%4d%4d%5d%5d%4d%4d%4d%4d",
				       lp->path, lp->dx, lp->dy,
				       lp->thresh[UW], lp->thresh[FOOD],
				       lp->thresh[SH], lp->thresh[GUN],
				       lp->thresh[PET], lp->thresh[IRON],
				       lp->thresh[DUST], lp->thresh[BAR],
				       lp->thresh[CRU], lp->thresh[LCM],
				       lp->thresh[HCM], lp->thresh[RAD]);
				break;
			case 2:
				lp = mp->lp;
				printw("%c%c%c%c%c%c%c%c%c%c%c%c",
				       vp->del[UW], vp->del[FOOD],
				       vp->del[SH], vp->del[GUN],
				       vp->del[PET], vp->del[IRON],
				       vp->del[DUST], vp->del[BAR],
				       vp->del[CRU], vp->del[LCM],
				       vp->del[HCM], vp->del[RAD]);
				printw("%5d%5d%4d%4d%5d%5d%4d%4d%4d%4d%4d%4d",
				       lp->cutoff[UW], lp->cutoff[FOOD],
				       lp->cutoff[SH], lp->cutoff[GUN],
				       lp->cutoff[PET], lp->cutoff[IRON],
				       lp->cutoff[DUST], lp->cutoff[BAR],
				       lp->cutoff[CRU], lp->cutoff[LCM],
				       lp->cutoff[HCM], lp->cutoff[RAD]);
				break;
			}
		} else if ((!planemode || mp->pln == NOPLANES) &&
		    	   ( unitmode  && mp->unt != NOUNITS)  &&
			   (!shipmode  || mp->shp == NOSHIPS)) {
			up = units[mp->unt];
			if ((vp = up->vp) == NULL)
				return;
			if (vp->val[COU] < 0)
				addstr("     ");
			else
				printw("%5d", vp->val[COU]);

			printw("  %-16.16s %5d%6d%6d%6d%5d%5d%5d%5d%5d%6d",
			       getnam(vp->val[COU]), vp->val[SH], vp->val[GUN],
			      vp->val[PET], vp->val[IRON], vp->val[DUST],
			       vp->val[BAR], vp->val[CRU], vp->val[LCM],
			       vp->val[HCM], vp->val[RAD]);
		} else if (!planemode || mp->pln == NOPLANES) {
			sp = ships[mp->shp];
			if ((vp = sp->vp) == NULL)
				return;
			if (vp->val[COU] < 0)
				addstr("     ");
			else
				printw("%5d", vp->val[COU]);

			printw("  %-16.16s %5d%6d%6d%6d%5d%5d%5d%5d%5d%6d",
			       getnam(vp->val[COU]), vp->val[SH], vp->val[GUN],
			       vp->val[PET], vp->val[IRON], vp->val[DUST],
			       vp->val[BAR], vp->val[CRU], vp->val[LCM],
			       vp->val[HCM], vp->val[RAD]);
		} else {
			pp = planes[mp->pln];
			if ((vp = pp->vp) == NULL)
				return;
			if (vp->val[COU] < 0)
				addstr("   ");
			else
				printw("%3d", vp->val[COU]);
			printw(" %4d  %-16.16s %4d,%-4d %c %4d%%%5d",
			       pp->number, pp->type, x, y, pp->wing,
			       vp->val[EFF], vp->val[MOB]);
			printw("%5d%5d%5d%4d%5d%5d",
			       vp->val[ATT], vp->val[DEF],
			       vp->val[TECH], vp->val[RANGE],
			       vp->val[SHIP], vp->val[NUKE]);
		}

	}
}
/*
 * censusheader - Display census header.
 */
censusheader(mode, lmode)
	register int mode, lmode;
{
	move(LINES - 5, 0);
	clrtoeol();
	switch (mode) {
	case 0:			    /* Sector Census Report */
	case 2:
	case 3:
	case 5:
	case 6:
	case 7:
		addstr("  sect des  eff mob uf uf *  ");
		addstr("civ  mil   uw food min gold fert oil uran work ter");
		break;

	case 1:			    /* Ship (census) Report */
		addstr("ship#  ship type         x,y   flt   ");
		addstr("eff   civ  mil   uw food  pln  mob  fuel");
		break;
	case 4:			    /* Unit (census) Report */
		addstr("unit#  unit type         x,y   army  ");
		addstr("eff   fort mob fuel food tech  retr  rad  xl");
		break;
	}

	move(LINES - 3, 0);
	clrtoeol();
	switch (mode) {
	case 0:			    /* Sector Commodities
					     * Report */
		switch (lmode) {
		case 0:
			addstr("cou sgpidbclhr sgpidbclhr ");
			addstr("sh  gun  pet iron dust  bar  cru  lcm  hcm rad avail");
			break;
		case 1:		    /* distribution levels */
			addstr("path------   dst    uw food");
			addstr("  sh gun pet iron dust bar cru lcm hcm rad");
			break;
		case 2:		    /* delivery levels */
			addstr("ufsgpidbclhr   uw food  sh gun");
			addstr(" pet iron dust bar cru lcm hcm rad");
			break;
		}
		break;
	case 1:			    /* Ship (cargo) Report */
		addstr("owner  country             ");
		addstr("sh   gun   pet  iron dust  bar  oil  lcm  hcm   rad");
		break;
	case 2:			    /* Plane (census) Report */
		addstr("cou    #     type             x,y   wng ");
		addstr("eff   mob  att  def tech ran ship nuke");
		break;
	case 3:
		addstr("Ships and planes in same location.");
		addstr("  Please toggle off one option.");
		break;
	case 4:			    /* Unit (cargo) Report */
		addstr("owner  country             ");
		addstr("sh   gun   pet  iron dust  bar  oil  lcm  hcm   rad");
		break;
	case 5:
		addstr("Ships and units in same location.");
		addstr("  Please toggle off one option.");
		break;
	case 6:
		addstr("Units and planes in same location.");
		addstr("  Please toggle off one option.");
		break;
	case 7:
		addstr("Ships, units  and planes in same location.");
		addstr("  Please toggle off two options.");
		break;
	}
}
/* getnam - Get Country name for cnum */
char   *
getnam(cnum)
	int     cnum;
{

	if (cnum == YOURS)
		return ("Your own");

	if (cnum == UNKNOWN)
		return ("Unknown");

	return (CountryNames[cnum]);
}
