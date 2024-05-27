/* ship.c - ship parsing and other shippy routines */

# include	"ve.h"
#include <memory.h>


/*
 * ship - Read and process ship reports.
 */
ship(fp)
	FILE   *fp;
{
	char    des;
	register int number;
	register int i;
	int     tx, ty;
	Sector *mp;
	Ship   *sp;
	Value  *vp;
	char    buf[BUFSIZ];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[26] != ',' || buf[3] == '#')
			continue;
		number = atoi(buf);
		buf[26] = ':';		    /* So we don't process
					     * this line again */

		if ((i = locship(number)) == NOSHIPS) {
			i = shipcount;
			if (++shipcount >= MAXSHIPS) {
				fputs("Ship vector overflow!\n", stderr);
				(void) fflush(stderr);
				exit(1);
			}
			sp = ships[i] = newship();
		} else {
			sp = ships[i];
			if ((mp = map[xoffset(sp->x)][yoffset(sp->y)]) &&
			    mp->shp != NOSHIPS)
				mp->shp = NOSHIPS;
		}

		if (sp->number != number) {
			strncpy(sp->type, &buf[5], 15);
			sp->type[15] = '\0';
			des = buf[5];
			if (des > 'Z')
				des -= ('a' - 'A');
			sp->des = des;
			sp->number = number;
			sp->x = atoi(&buf[23]);
			sp->y = atoi(&buf[27]);
			sp->fleet = buf[32];
		}
		if (sp->vp == NULL) {
			vp = sp->vp = newval();
			vp->val[COU] = YOURS;
			vp->val[EFF] = atoi(&buf[34]);
		}
		vp = sp->vp;
		sscanf(&buf[40], "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd",
		       &vp->val[CIV], &vp->val[MIL], &vp->val[UW],
		       &vp->val[FOOD], &vp->val[PL], &vp->val[HE], 
		       &vp->val[XL], &vp->val[LN],
		       &vp->val[MOB], &vp->val[FUEL], &vp->val[TECH]);
		mp = map[tx = xoffset(sp->x)][ty = yoffset(sp->y)];
		if (mp == NULL)
			map[tx][ty] = mp = newmap();
		if (mp->shp == NOSHIPS)
			mp->shp = i;
		if (mp->own == 0)
			mp->own = 1;
		/*
		 * now guess that anything we don't know
		 * about is sea
		 */
		tx = (sp->x);
		ty = (sp->y);
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
 * cargo - Read and process cargo reports.
 */
cargo(fp)
	FILE   *fp;
{
	char    des;
	register int number;
	register int i;
	int     tx, ty;
	Sector *mp;
	Ship   *sp;
	Value  *vp;
	char    buf[BUFSIZ];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[14] != ',' || buf[3] == '#')
			continue;
		number = atoi(buf);
		buf[14] = ':';		    /* So we don't process
					     * this line again */

		if ((i = locship(number)) == NOSHIPS) {
			i = shipcount;
			if (++shipcount >= MAXSHIPS) {
				fputs("Ship vector overflow!\n", stderr);
				(void) fflush(stderr);
				exit(1);
			}
			sp = ships[i] = newship();
		} else {
			sp = ships[i];
			if ((mp = map[xoffset(sp->x)][yoffset(sp->y)]) &&
			    mp->shp != NOSHIPS)
				mp->shp = NOSHIPS;
		}

		if (sp->number != number) {
			strncpy(sp->type, &buf[5], 5);
			sp->type[5] = '\0';
			des = buf[5];
			if (des > 'Z')
				des -= ('a' - 'A');
			sp->des = des;
			sp->number = number;
			sp->x = atoi(&buf[11]);
			sp->y = atoi(&buf[15]);
			sp->fleet = buf[20];
		}
		if (sp->vp == NULL) {
			vp = sp->vp = newval();
			vp->val[COU] = YOURS;
			vp->val[EFF] = atoi(&buf[22]);
		}
		vp = sp->vp;
		sscanf(&buf[39], "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd",
		       &vp->val[SH], &vp->val[GUN], &vp->val[PET],
		       &vp->val[IRON], &vp->val[DUST], &vp->val[BAR],
		       &vp->val[CRU], &vp->val[LCM], &vp->val[HCM],
		       &vp->val[RAD]);
		mp = map[tx = xoffset(sp->x)][ty = yoffset(sp->y)];
		if (mp == NULL)
			map[tx][ty] = mp = newmap();
		if (mp->shp == NOSHIPS)
			mp->shp = i;
		if (mp->own == 0)
			mp->own = 1;
		/*
		 * now guess that anything we don't know
		 * about is sea
		 */
		tx = (sp->x);
		ty = (sp->y);
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
 * findship - Find ship or first ship of fleet.
 */
findship(theship, fleet)
	register int theship;
	register char fleet;
{
	register int i;

	if (theship == UNKNOWN) {
		for (i = 0; i < shipcount; i++)
			if (ships[i]->fleet == fleet) {
				theship = ships[i]->number;
				break;
			}
	}
	if (theship == UNKNOWN)
		return (NOSHIPS);
	return (locship(theship));
}
/*
 * locateship - Locate arbitrary ship and move display window.
 */
void
locateship(int *x, int *y, int sflg)
{
	register int number;
	register int i;
	char    fleet;
	char    buf[BUFSIZ];

	ve_getline(buf, "Ship number: ", NOX);
	if (*buf) {
		fleet = buf[0];
		if (fleet >= '0' && fleet <= '9')
			number = atoi(buf);
		else
			number = UNKNOWN;
		if ((i = findship(number, fleet)) == NOPLANES) {
			putline("No info on ship");
			return;
		}
		*x = ships[i]->x;
		*y = ships[i]->y;
		center(*x, *y, FALSE);
		map[xoffset(*x)][yoffset(*y)]->shp = i;
		if (!shipmode) {
			shipmode = TRUE;
			mapdr(sflg);
		}
		censusinfo(*x, *y);
		return;
	}
}


/*
 * firstship - Locate first ship at x,y.
 */
firstship(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Ship   *sp;

	if (!shipmode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	mp = map[mx][my];
	if (mp == NULL)
		return;
	if (mp->shp == NOSHIPS)
		return;
	for (i = 0; i < shipcount; i++) {
		sp = ships[i];
		if (xoffset(sp->x) == mx && yoffset(sp->y) == my) {
			mp->shp = i;
			return;
		}
	}
}
/*
 * nextship - Advance ship index to next ship at x,y.
 */
nextship(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Ship   *sp;

	if (!shipmode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	mp = map[mx][my];
	if (mp == NULL)
		return;
	if (mp->shp == NOSHIPS)
		return;

	for (i = mp->shp + 1; i < shipcount; i++) {
		sp = ships[i];
		if (xoffset(sp->x) == mx && yoffset(sp->y) == my) {
			mp->shp = i;
			return;
		}
	}

	for (i = 0; i < mp->shp; i++) {
		sp = ships[i];
		if (xoffset(sp->x) == mx && yoffset(sp->y) == my) {
			mp->shp = i;
			return;
		}
	}
}
/*
 * previousship - Back up to previous ship at x,y.
 */
previousship(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Ship   *sp;

	if (!shipmode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	mp = map[mx][my];
	if (mp == NULL)
		return;
	if (mp->shp == NOSHIPS)
		return;

	for (i = mp->shp - 1; i >= 0; i--) {
		sp = ships[i];
		if (xoffset(sp->x) == mx && yoffset(sp->y) == my) {
			mp->shp = i;
			return;
		}
	}

	for (i = shipcount - 1; i > mp->shp; i--) {
		sp = ships[i];
		if (xoffset(sp->x) == mx && yoffset(sp->y) == my) {
			mp->shp = i;
			return;
		}
	}
}
locship(n)
	int     n;
{
	register int i;

	for (i = 0; i < shipcount; i++)
		if (ships[i]->number == n)
			return i;
	return NOSHIPS;
}

Ship   *
newship()
{
	Ship   *sp;

	if ((sp = (Ship *) calloc(1, sizeof(Ship))) == NULL)
		error(1, "Out of memory in ship");
	sp->number = UNKNOWN;
	return sp;
}

/*
 * format the ship for 1-line display
 */
char *
fmtship(Ship *s, char *buf)
{
	/* ship# type ---x,y--- flt %eff mob civ mil -uw gun -sh */
	Value *vp = s->vp;
	sprintf(buf, /* ship# */ "%5d "
	             /* type  */ "%3s "
				 /* x,y   */ "%4d,%-4d "
				 /* flt   */ "%c  "
				 /* %eff  */ "%%%-3d "
				 /* mob   */ "%3d "
				 /* civ   */ "%3d "
				 /* mil   */ "%3d "
				 /* -uw   */ "%3d "
				 /* gun   */ "%3d "
				 /* sh    */ "%3d ",
				 s->number, s->type, s->x, s->y, s->fleet,
				 vp->val[EFF], vp->val[MOB],
				 vp->val[CIV], vp->val[MIL], vp->val[UW], 
				 vp->val[GUN], vp->val[SH]);
	return buf;
}

/*
 * format all ships
 */
char *fmtships(int *rc)
{
	/* caller frees */
	short n;
	char *buf = calloc(60, shipcount+2);
	/* ship# type ---x,y--- flt %eff mob civ mil -uw gun -sh */
	strcpy(buf,        "ship# type    x,y    flt %eff mob civ mil  uw gun  sh");
	strcpy(buf+(1*60), "----- ---- --------- --- ---- --- --- --- --- --- ---");

	for (n=0; n<shipcount; ++n) {
		if (ships[n]->number < 0) continue;
		if (ships[n]->vp->val[COU] != YOURS) continue;
		fmtship(ships[n], buf+((*rc)*60));
		++*rc;
	}
	return buf;
}

