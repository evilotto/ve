/* plane.c - plane routines */

#include "ve.h"

/*
 * findplane Find plane or first plane of wing.
 */
int
findplane(plane, wing)
	register int plane;
	register char wing;
{
	register int i;

	if (plane == UNKNOWN) {
		for (i = 0; i < planecount; i++)
			if (planes[i]->wing == wing) {
				plane = planes[i]->number;
				break;
			}
	}
	if (plane == UNKNOWN)
		return (NOPLANES);
	return (locplane(plane));
}
/*
 * firstplane Locate first plane at x,y.
 */
void
firstplane(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Plane  *pp;

	if (!planemode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	maxxy(mx, my);
	mp = map[mx][my];
	if (mp == NULL || mp->pln == NOPLANES)
		return;
	for (i = 0; i < planecount; i++) {
		pp = planes[i];
		if (xoffset(pp->x) == mx && yoffset(pp->y) == my) {
			mp->pln = i;
			return;
		}
	}
}
/*
 * locateplane - Locate arbitrary plane and move display window.
 */
void
locateplane(int *px, int *py, int sflg)
{
	register int number;
	register int i;
	char    wing;
	char    buf[BUFSIZ];

	ve_getline(buf, "Plane number: ", NOX);
	if (*buf) {
		wing = buf[0];
		if (wing >= '0' && wing <= '9')
			number = atoi(buf);
		else
			number = UNKNOWN;
		if ((i = findplane(number, wing)) == NOPLANES) {
			putline("No info on plane");
			return;
		}
		*px = planes[i]->x;
		*py = planes[i]->y;
		center(*px, *py, FALSE);
		map[xoffset(*px)][yoffset(*py)]->pln = i;
		if (!planemode) {
			planemode = TRUE;
			mapdr(sflg);
		}
		censusinfo(*px, *py);
		return;
	}
}
/*
 * nextplane - Advance plane index to next plane at x,y.
 */
void
nextplane(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Plane  *pp;

	if (!planemode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	mp = map[mx][my];
	if (mp == NULL || mp->pln == NOPLANES)
		return;

	for (i = mp->pln + 1; i < planecount; i++) {
		pp = planes[i];
		if (xoffset(pp->x) == mx && yoffset(pp->y) == my) {
			mp->pln = i;
			return;
		}
	}

	for (i = 0; i < mp->pln; i++) {
		pp = planes[i];
		if (xoffset(pp->x) == mx && yoffset(pp->y) == my) {
			mp->pln = i;
			return;
		}
	}
}
/*
 * plane - Read and process plane reports.
 */
void
plane(inpf)
	FILE   *inpf;
{
	char    des;
	register int number;
	register int i;
	int     tx, ty;
	Sector *mp;
	Plane  *pp;
	Value  *vp;
	char    buf[80];

	while (fgets(buf, sizeof buf, inpf) != NULL) {
		if (buf[29] != ',' || buf[3] == '#')
			continue;
		number = atoi(buf);
		buf[29] = ':';		    /* So we don't process
					     * this line again */

		if ((i = locplane(number)) == NOPLANES) {
			i = planecount;
			if (++planecount >= MAXPLANES) {
				fputs("Plane vector overflow!\n", stderr);
				(void) fflush(stderr);
				exit(1);
			}
			pp = planes[i] = newplane();
		} else {
			pp = planes[i];
			if ((mp = map[xoffset(pp->x)][yoffset(pp->y)]) &&
			    mp->pln == NOPLANES)
				mp->pln = NOPLANES;
		}

		if (pp->number != number) {
			strncpy(pp->type, &buf[5], 15);
			pp->type[15] = '\0';
			des = buf[5];
			if (des > 'Z')
				des -= ('a' - 'A');
			pp->des = des;
			pp->number = number;
			pp->x = atoi(&buf[25]);
			pp->y = atoi(&buf[30]);
			pp->wing = buf[35];
		}
		if (pp->vp == NULL) {
			vp = pp->vp = newval();
			vp->val[COU] = YOURS;
			vp->val[EFF] = atoi(&buf[37]);
		}
		sscanf(&buf[42], "%hd%hd%hd%hd%hd%hd%hd",
		       &vp->val[MOB], &vp->val[ATT], &vp->val[DEF],
		       &vp->val[TECH], &vp->val[RANGE],
		       &vp->val[SHIP], &vp->val[NUKE]);

		mp = map[tx = xoffset(pp->x)][ty = yoffset(pp->y)];
		if (mp == NULL)
			map[tx][ty] = mp = newmap();
		if (mp->pln == NOPLANES)
			mp->pln = i;
		if (mp->own == 0)
			mp->own = 1;
	}
}
/*
 * previousplane - Back up to previous plane at x,y.
 */
void
previousplane(x, y)
	int     x, y;
{
	register int mx, my;
	register int i;
	Sector *mp;
	Plane  *pp;

	if (!planemode)
		return;
	mx = xoffset(x);
	my = yoffset(y);
	mp = map[mx][my];
	if (mp == NULL || mp->pln == NOPLANES)
		return;

	for (i = mp->pln - 1; i >= 0; i--) {
		pp = planes[i];
		if (xoffset(pp->x) == mx && yoffset(pp->y) == my) {
			mp->pln = i;
			return;
		}
	}

	for (i = planecount - 1; i > mp->pln; i--) {
		pp = planes[i];
		if (xoffset(pp->x) == mx && yoffset(pp->y) == my) {
			mp->pln = i;
			return;
		}
	}
}
int
locplane(n)
	int     n;
{
	register int i;

	for (i = 0; i < planecount; i++)
		if (planes[i]->number == n)
			return i;
	return NOPLANES;
}

Plane  *
newplane()
{
	Plane  *pp;

	if ((pp = (Plane *) calloc(1, sizeof(Plane))) == NULL)
		error(1, "Out of memory in plane");
	pp->number = UNKNOWN;
	return pp;
}
