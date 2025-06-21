/* radar.c - radar routines */

#include	"ve.h"

void shiprad(char des, int tx, int ty);
void satrad(int tx, int ty);
int nextscan(FILE *fp);
int readradarscan(FILE *fp, char rm[RADARSIZE][RADARSIZE], int *xr, int *yr);

void
editradarscan(rm, xr, yr, xp, yp)
	char    rm[RADARSIZE][RADARSIZE];
	int    *yr, *xr;
	int    *xp, *yp;
{
	register int x, y;
	register int xs = 0, ys = 0;

	for (x = 0; x <= *xr; x++)
		for (y = 0; y <= *yr; y++) {
			if (rm[x][y] == '0') {
				xs = x;
				ys = y;
				break;
			}
		}
	for (x = 0; x <= *xr; x++)
		for (y = 0; y <= *yr; y++) {
			if (rm[x][y] >= 'A' && rm[x][y] <= 'Z') {
				shiprad(rm[x][y], *xp - xs + x, *yp - ys + y);
				rm[x][y] = '$';
			} else if (rm[x][y] == '$')
				satrad(*xp - xs + x, *yp - ys + y);
			else if (rm[x][y] == '0')
				rm[x][y] = '$';
			else if (rm[x][y] == '/')
				rm[x][y] = ' ';
			merge(rm[x][y], xoffset(*xp - xs + x),
			      yoffset(*yp - ys + y), FALSE);
			maxxy(xoffset(*xp - xs + x), yoffset(*yp - ys + y));
		}
}


void
presetradarmap(rm)
	char    rm[RADARSIZE][RADARSIZE];
{
	register int x, y;

	for (x = 0; x < RADARSIZE; x++)
		for (y = 0; y < RADARSIZE; y++)
			rm[x][y] = ' ';
}
void
radarscan(fp)
	FILE   *fp;
{
	int     xp, yp;
	float   rp;
	int     xr, yr;
	char    radarmap[RADARSIZE][RADARSIZE];

	while (nextscan(fp)) {
		if (!readradarlines(fp, &xp, &yp, &rp)) {
			if (isamap(fp)) {
				if (!readradarlines(fp, &xp, &yp, &rp))
					continue;
			} else
				continue;
		}
		if (rp < 0.6)
			continue;
		presetradarmap(radarmap);
		if (!readradarscan(fp, radarmap, &xr, &yr))
			continue;
		editradarscan(radarmap, &xr, &yr, &xp, &yp);
	}
}
int
nextscan(fp)
	FILE   *fp;
{
	char    c;

	c = getc(fp);

	if (c == EOF)
		return (0);

	if (c == '-' || (c >= '0' && c <= '9')) {	/* It's a land Map */
		ungetc(c, fp);
		return (1);
	}
	while (c != EOF && c != '\n' && c != '#')	/* skip ship info */
		c = getc(fp);

	if (c != '#')
		return (1);		    /* Not a radar line */

	if (!isdigit(getc(fp)))
		return (1);		    /* Not a ship # */

	while (isdigit(getc(fp)));	    /* Skip the ship # */

	if (getc(fp) != 'a')
		return (1);		    /* check for "at " */
	if (getc(fp) != 't')
		return (1);
	if (getc(fp) != ' ')
		return (1);

	return (1);			    /* success at last */
}
int
readradarlines(fp, xp, yp, rp)
	FILE   *fp;
	int    *xp, *yp;
	float  *rp;
{
	static char format[] =
#ifdef	BAROMETER
	"%d,%d efficiency %*d%%, barometer at %*d, max range %f";
#else
	"%d,%d efficiency %*d%%, max range %f";
#endif

	if (fscanf(fp, format, xp, yp, rp) != 3)
		return (FALSE);
	pitchline(fp);
	return (TRUE);
}
int
readradarscan(fp, rm, xr, yr)
	FILE   *fp;
	char    rm[RADARSIZE][RADARSIZE];
	int    *xr, *yr;
{
	register int c, d;
	register int x, y;

	*xr = x = 0;
	*yr = y = 0;

	while ((c = getc(fp)) != EOF) {
		if (x == 0) {
/*			if (c == '-' || (c >= '0' && c <= '9')) {
				ungetc(c, fp);
				return (*xr > 0 && --(*yr) > 0);
			}*/
			if (!isspace(c)) {
				d = getc(fp);
				if (!isspace(d)) {
					ungetc(d, fp);
					return (*xr > 0 && --(*yr) > 0);
				}
				rm[x++][y] = c;
				c = d;
			}
		}
		if (c == '\n') {
			if (++y >= RADARSIZE)
				return (FALSE);
			*xr = max(*xr, x - 1);
			*yr = max(*yr, y);
			x = 0;
			if ((c = getc(fp)) == '\n')
				return (*xr > 0 && --(*yr) > 0);
			else
				ungetc(c, fp);
		} else {
			rm[x][y] = c;
			if (++x >= RADARSIZE)
				return (FALSE);
		}
	}
	(*yr)--;
	return (*xr > 0 && *yr > 0);
}
/*
 * shiprad - Process ship sightings by radar.
 */
void
shiprad(des, tx, ty)
	char    des;
	int     tx, ty;
{
	register int i;
	Sector *mp;
	Ship   *sp;

	if ((mp = map[xoffset(tx)][yoffset(ty)]) && mp->shp != NOSHIPS)
		return;
	i = shipcount;
	if (++shipcount >= MAXSHIPS) {
		fputs("Ship vector overflow!\n", stderr);
		(void) fflush(stderr);
		exit(1);
	}
	sp = ships[i] = newship();
	strcpy(sp->type, "Radar Echo");
	sp->des = des;
	sp->fleet = ' ';
	sp->x = tx;
	sp->y = ty;

	if (mp == NULL)
		map[xoffset(tx)][yoffset(ty)] = mp = newmap();
	if (mp->shp == NOSHIPS)
		mp->shp = i;
	if (mp->own == 0)
		mp->own = 1;
}
/*
 * satrad - Process satellite sightings by radar.
 */
void
satrad(tx, ty)
	int     tx, ty;
{
	register int i;
	Sector *mp;
	Plane  *pp;

	if ((mp = map[xoffset(tx)][yoffset(ty)]) && mp->pln != NOPLANES)
		return;
	i = planecount;
	if (++planecount >= MAXPLANES) {
		fputs("Plane vector overflow!\n", stderr);
		(void) fflush(stderr);
		exit(1);
	}
	pp = planes[i] = newplane();
	strcpy(pp->type, "Satellite");
	pp->des = '$';
	pp->wing = ' ';
	pp->x = tx;
	pp->y = ty;

	/* This is silly but it won't display without it */
	pp->vp = newval();
	pp->vp->val[COU] = UNKNOWN;

	if (mp == NULL)
		map[xoffset(tx)][yoffset(ty)] = mp = newmap();
	if (mp->pln == NOPLANES)
		mp->pln = i;
}
