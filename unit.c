/* unit.c - unit parsing and other unitpy routines */

# include	"ve.h"

Unit * newunit()
{
	Unit   *up;

	if ((up = (Unit *) calloc(1, sizeof(Unit))) == NULL)
		error(1, "Out of memory in unit");
	up->number = UNKNOWN;
	return up;
}

/*
 * unit - Read and process unit reports.
 */
unit(fp)
	FILE   *fp;
{
	char    des;
	register int number;
	register int i;
	int     tx, ty;
	Sector *mp;
	Unit   *up;
	Value  *vp;
	char    buf[BUFSIZ];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[28] != ',' || buf[3] == '#')
			continue;
		number = atoi(buf);
		buf[28] = ':';		    /* So we don't process
					     * this line again */

		if ((i = locunit(number)) == NOUNITS) {
			i = unitcount;
			if (++unitcount >= MAXUNITS) {
				fputs("Unit vector overflow!\n", stderr);
				(void) fflush(stderr);
				exit(1);
			}
			up = units[i] = newunit();
		} else {
			up = units[i];
			if ((mp = map[xoffset(up->x)][yoffset(up->y)]) &&
			    mp->unt != NOUNITS)
				mp->unt = NOUNITS;
		}

		if (up->number != number) {
			strncpy(up->type, &buf[5], 15);
			up->type[15] = '\0';
			des = buf[5];
			if (des > 'Z')
				des -= ('a' - 'A');
			up->des = des;
			up->number = number;
			up->x = atoi(&buf[24]);
			up->y = atoi(&buf[29]);
			up->army = buf[33];
		}
		if (up->vp == NULL) {
			vp = up->vp = newval();
			vp->val[COU] = YOURS;
			vp->val[EFF] = atoi(&buf[35]);
		}
		vp = up->vp;
		sscanf(&buf[40], "%hd%hd%hd%hd%hd",
		       &vp->val[FORT], &vp->val[MOB], &vp->val[FOOD],
		       &vp->val[FUEL], &vp->val[TECH]);
		vp->val[RETR] = atoi(&buf[64]);
		sscanf(&buf[68],"%hd%hd", &vp->val[RADIUS], &vp->val[XL]);
		mp = map[tx = xoffset(up->x)][ty = yoffset(up->y)];
		if (mp == NULL)
			map[tx][ty] = mp = newmap();
		if (mp->unt == NOUNITS)
			mp->unt = i;
		if (mp->own == 0)
			mp->own = 1;
		/*
		 * now guess that anything we don't know
		 * about is sea
		 */
		tx = (up->x);
		ty = (up->y);
		maxxy(xoffset(tx + 2), yoffset(ty + 1));
		maxxy(xoffset(tx - 2), yoffset(ty - 1));

		merge('$', xoffset(tx + 1), yoffset(ty + 1), FALSE);
		merge('$', xoffset(tx - 1), yoffset(ty + 1), FALSE);
		merge('$', xoffset(tx + 2), yoffset(ty), FALSE);
		merge('$', xoffset(tx), yoffset(ty), FALSE);
		merge('$', xoffset(tx - 2), yoffset(ty), FALSE);
		merge('$', xoffset(tx + 1), yoffset(ty - 1), FALSE);
		merge('$', xoffset(tx - 1), yoffset(ty - 1), FALSE);

	}
}


/*
 * lcargo - Read and process lcargo reports.
 */
lcargo(fp)
	FILE   *fp;
{
	char    des;
	register int number;
	register int i;
	int     tx, ty;
	Sector *mp;
	Unit   *up;
	Value  *vp;
	char    buf[BUFSIZ];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[26] != ',' || buf[3] == '#')
			continue;
		number = atoi(buf);
		buf[26] = ':';		    /* So we don't process
					     * this line again */

		if ((i = locunit(number)) == NOUNITS) {
			i = unitcount;
			if (++unitcount >= MAXUNITS) {
				fputs("Unit vector overflow!\n", stderr);
				(void) fflush(stderr);
				exit(1);
			}
			up = units[i] = newunit();
		} else {
			up = units[i];
			if ((mp = map[xoffset(up->x)][yoffset(up->y)]) &&
			    mp->unt != NOUNITS)
				mp->unt = NOUNITS;
		}

		if (up->number != number) {
			strncpy(up->type, &buf[5], 15);
			up->type[15] = '\0';
			des = buf[5];
			if (des > 'Z')
				des -= ('a' - 'A');
			up->des = des;
			up->number = number;
			up->x = atoi(&buf[22]);
			up->y = atoi(&buf[27]);
			up->army = buf[33];
		}
		if (up->vp == NULL) {
			vp = up->vp = newval();
			vp->val[COU] = YOURS;
			vp->val[EFF] = atoi(&buf[35]);
		}
		vp = up->vp;
		sscanf(&buf[39], "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd",
		       &vp->val[SH], &vp->val[GUN], &vp->val[PET],
		       &vp->val[IRON], &vp->val[DUST], &vp->val[BAR],
		       &vp->val[CRU], &vp->val[LCM], &vp->val[HCM],
		       &vp->val[RAD]);
		mp = map[tx = xoffset(up->x)][ty = yoffset(up->y)];
		if (mp == NULL)
			map[tx][ty] = mp = newmap();
		if (mp->unt == NOUNITS)
			mp->unt = i;
		if (mp->own == 0)
			mp->own = 1;
		/*
		 * now guess that anything we don't know
		 * about is sea
		 */
		tx = (up->x);
		ty = (up->y);
		maxxy(xoffset(tx + 2), yoffset(ty + 1));
		maxxy(xoffset(tx - 2), yoffset(ty - 1));

		merge('$', xoffset(tx + 1), yoffset(ty + 1), FALSE);
		merge('$', xoffset(tx - 1), yoffset(ty + 1), FALSE);
		merge('$', xoffset(tx + 2), yoffset(ty), FALSE);
		merge('$', xoffset(tx), yoffset(ty), FALSE);
		merge('$', xoffset(tx - 2), yoffset(ty), FALSE);
		merge('$', xoffset(tx + 1), yoffset(ty - 1), FALSE);
		merge('$', xoffset(tx - 1), yoffset(ty - 1), FALSE);

	}
}
/*
 * findunit - Find unit or first unit of army.
 */
findunit(theunit, army)
	register int theunit;
	register char army;
{
	register int i;

	if (theunit == UNKNOWN) {
		for (i = 0; i < unitcount; i++)
			if (units[i]->army == army) {
				theunit = units[i]->number;
				break;
			}
	}
	if (theunit == UNKNOWN)
		return (NOUNITS);
	return (locunit(theunit));
}


/*
 * firstunit - Locate first unit at x,y.
 */
firstunit(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Unit   *up;

	if (!unitmode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	mp = map[mx][my];
	if (mp == NULL)
		return;
	if (mp->unt == NOUNITS)
		return;
	for (i = 0; i < unitcount; i++) {
		up = units[i];
		if (xoffset(up->x) == mx && yoffset(up->y) == my) {
			mp->unt = i;
			return;
		}
	}
}
/*
 * locateunit - Locate arbitrary unit and move display window.
 */
locateunit(sx, sy, sflg)
	register int *sx, *sy;
	int     sflg;
{

	register int number;
	register int i;
	char    army;
	char    buf[BUFSIZ];

	ve_getline(buf, "Unit number: ", NOX);
	if (*buf) {
		army = buf[0];
		if (army >= '0' && army <= '9')
			number = atoi(buf);
		else
			number = UNKNOWN;
		if ((i = findunit(number, army)) == NOUNITS) {
			putline("No info on unit");
			return;
		}
		*sx = units[i]->x;
		*sy = units[i]->y;
		center(*sx, *sy, FALSE);
		map[xoffset(*sx)][yoffset(*sy)]->unt = i;
		if (!unitmode) {
			unitmode = TRUE;
			mapdr(sflg);
		}
		censusinfo(*sx, *sy);
		return;
	}
}
/*
 * nextunit - Advance unit index to next unit at x,y.
 */
nextunit(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Unit   *up;

	if (!unitmode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	mp = map[mx][my];
	if (mp == NULL)
		return;
	if (mp->unt == NOUNITS)
		return;

	for (i = mp->unt + 1; i < unitcount; i++) {
		up = units[i];
		if (xoffset(up->x) == mx && yoffset(up->y) == my) {
			mp->unt = i;
			return;
		}
	}

	for (i = 0; i < mp->unt; i++) {
		up = units[i];
		if (xoffset(up->x) == mx && yoffset(up->y) == my) {
			mp->unt = i;
			return;
		}
	}
}
/*
 * previousunit - Back up to previous unit at x,y.
 */
previousunit(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Unit   *up;

	if (!unitmode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	mp = map[mx][my];
	if (mp == NULL)
		return;
	if (mp->unt == NOUNITS)
		return;

	for (i = mp->unt - 1; i >= 0; i--) {
		up = units[i];
		if (xoffset(up->x) == mx && yoffset(up->y) == my) {
			mp->unt = i;
			return;
		}
	}

	for (i = unitcount - 1; i > mp->unt; i--) {
		up = units[i];
		if (xoffset(up->x) == mx && yoffset(up->y) == my) {
			mp->unt = i;
			return;
		}
	}
}
locunit(n)
	int     n;
{
	register int i;

	for (i = 0; i < unitcount; i++)
		if (units[i]->number == n)
			return i;
	return NOUNITS;
}
