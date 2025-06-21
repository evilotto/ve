/* map.c - map routines */

#include	"ve.h"

/*
 * readmap - Read and process the map file.  This is a real kludge where you
 *           try to figure out where you are on the map by looking at the
 *           coordinates along the edge of the map.
 */
void
readmap(fp)
	FILE   *fp;
{
	register int x, y;		    /* x,y map coordinates */
	register int transx, transy;	    /* transformed x,y */
	int     sign;			    /* Sign of number being
					     * converted */
	char    minb[5];		    /* Number buffers for
					     * min and max x */
	char    maxb[5];
	char   *sp = minb;
	char   *lp = maxb;
	char   *bp;
	char    buf[BUFSIZ];
	int	three_digits = 0;

	while ((bp = fgets(buf, sizeof buf, fp)) != NULL)
		if (*bp != '\n')
			break;
	/* Now determine the minimum x value */
	*sp++ = buf[5];
	*lp++ = buf[strlen(buf) - 2];
	sign = (index(buf, '-')) ? -1 : 1;
	fgets(buf, sizeof buf, fp);
	*sp++ = buf[5];
	*lp++ = buf[strlen(buf) - 2];
	fgets(buf, sizeof buf, fp);
	if (isdigit(buf[5]) != 0) {
		*sp++ = buf[5];
		*lp++ = buf[strlen(buf) - 2];
		three_digits = 1;
	}
	*sp = 0;
	*lp = 0;
	minx = atoi(minb);
	maxx = atoi(maxb);

	if (sign == -1) {
		if (minx > 0)
			minx *= -1;
	} else {
		if (minx > maxx) {
			maxx *= -1;
			minx *= -1;
		}
	}

	if (three_digits)
		fgets(buf, sizeof buf, fp);
	miny = atoi(buf);

	for (y = miny; buf[3] != ' '; y++) {
		transy = yoffset(y);
		for (bp = &buf[5], x = minx;
		     x <= maxx; x++, bp++) {
			transx = xoffset(x);
			if (*bp != ' ') {
				merge(*bp, transx, transy, FALSE);
				maxxy(transx, transy);
			}
		}
		fgets(buf, sizeof buf, fp);
	}
}
/*
 * mapdr - Display map.
 */
void
mapdr(int sflg)
{
	register int x, y;
	register char des;
	register Sector *mp;

	for (y = starty; y <= starty + MLINES; y++)
		for (x = startx; x <= startx + MCOLS; x++) {
			int hlcolor = 0;
			int ty = yoffset(y), tx = xoffset(x);
			mvaddch(y - starty, x - startx, ' ');
			if (!VALID(x, y))
				continue;
			mp = map[tx][ty];
			if (mp == NULL)
				continue;
			if      (unitmode && mp->unt != NOUNITS)
				des = units[mp->unt]->des;
			else if (shipmode && mp->shp != NOSHIPS) {
				des = ships[mp->shp]->des;
				if (ships[mp->shp]->vp == NULL) nextship(x, y);
			} else if (planemode && mp->pln != NOPLANES)
				des = planes[mp->pln]->des;
			else if (sflg && mp->surv)
				des = mp->surv;
			else
				des = mp->des;
			if (ovl[tx][ty] != NULL) {
				attron(ovl[tx][ty]->color);
			}
			mvaddch(y - starty, x - startx - 1,
				(mp->mark) ? mp->mark : ' ');
			if (unitmode) {
				int iff = liff(x, y);
				switch (iff) {
					case IFF_ENEMY: hlcolor = COLOR_PAIR(LCOLOR_ENEMY); break;
					case IFF_FRIEND: hlcolor = COLOR_PAIR(LCOLOR_FRIEND); break;
					case IFF_BOTH: hlcolor = COLOR_PAIR(LCOLOR_BOTH); break;
				}
			}
			if (shipmode) {
				int iff = niff(x, y);
				switch (iff) {
					case IFF_ENEMY: hlcolor = COLOR_PAIR(NCOLOR_ENEMY); break;
					case IFF_FRIEND: hlcolor = COLOR_PAIR(NCOLOR_FRIEND); break;
					case IFF_BOTH: hlcolor = COLOR_PAIR(NCOLOR_BOTH); break;
				}
			}
			if (des) {
				if (hlcolor) attron(hlcolor);
				if (mp->own == 2)
					standout();
				mvaddch(y - starty, x - startx, des);
				if (mp->own == 2)
					standend();
				if (hlcolor) attroff(hlcolor);
			}
			if (ovl[tx][ty] != NULL) {
				attroff(ovl[tx][ty]->color);
			}
			hlcolor = 0;
		}
}

/*
 * distance - measure straight-line distance between two points
 */
int
distance(int sx, int sy, int ex, int ey)
{
	int dx = abs(ex-sx);
	int dy = abs(ey-sy);
	return (dy + max(0, (dx-dy)/2));
}

void
drawrange(int x, int y, int r)
{
	int mx, my;
	for (mx = x-2*r; mx <= x+2*r; ++mx) {
		for (my = y-2*r; my <= y+2*r; ++my) {
			if (distance(x, y, mx, my) <= r) {
				setovl(mx, my, COLOR_PAIR(GCOLOR_RANGE));
			}
		}
	}
}

/*
 * niff - naval identify friend-or-foe
 * returns IFF_FRIEND, ENEMY, BOTH, NONE
 */
int
niff(int x, int y)
{
	int iff = IFF_NONE;
	Sector *sp = map[xoffset(x)][yoffset(y)];
	if (sp == NULL) return IFF_NONE;
	int ship = sp->shp;
	if (ship == NOSHIPS) return IFF_NONE;
	do {
		if (ships[sp->shp]->vp != NULL) {
			if (ships[sp->shp]->vp->val[COU] == YOURS) {
				iff |= IFF_FRIEND;
			} else {
				iff |= IFF_ENEMY;
			}
		}
		nextship(x, y);
	} while (sp->shp != ship);
	return iff;
}
/*
 * liff - land identify friend-or-foe
 * returns IFF_FRIEND, ENEMY, BOTH, NONE
 */
int
liff(int x, int y)
{
	int iff = IFF_NONE;
	Sector *sp = map[xoffset(x)][yoffset(y)];
	if (sp == NULL) return IFF_NONE;
	int land = sp->unt;
	if (land == NOUNITS) return IFF_NONE;
	do {
		if (units[sp->unt]->vp != NULL) {
			if (units[sp->unt]->vp->val[COU] == YOURS) {
				iff |= IFF_FRIEND;
			} else {
				iff |= IFF_ENEMY;
			}
		}
		nextunit(x, y);
	} while (sp->unt != land);
	return iff;
}
Overlay  *
newovl()
{
	Overlay  *op;

	op = (Overlay *) calloc(1, sizeof(*op));
	if (op == NULL)
		error(1, "Out of memory in newovl");
	return op;
}
void
setovl(int x, int y, short color)
{
	int tx = xoffset(x);
	int ty = yoffset(y);
	if (ovl[tx][ty] == NULL) {
		ovl[tx][ty] = newovl();
	}
	ovl[tx][ty]->color = color;
}
void
clearovl()
{
	int x, y;
	for (x=0; x<MAPSIZE; ++x) {
		for (y=0; y<MAPSIZE; ++y) {
			if (ovl[x][y] != NULL) {
				ovl[x][y]->color = 0;
			}
		}
	}
}




