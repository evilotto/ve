/* map.c - map routines */

#include	"ve.h"

/*
 * readmap - Read and process the map file.  This is a real kludge where you
 *           try to figure out where you are on the map by looking at the
 *           coordinates along the edge of the map.
 */
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
mapdr(sflg)
	register int sflg;
{
	register int x, y;
	register int tx, ty;
	register char des;
	register Sector *mp;

	for (y = starty, ty = yoffset(y);
	     y <= starty + MLINES; y++, ty = (++ty) % MAPSIZE)
		for (x = startx, tx = xoffset(x);
		     x <= startx + MCOLS; x++, tx = (++tx) % MAPSIZE) {
			mvaddch(y - starty, x - startx, ' ');
			if (!VALID(x, y))
				continue;
			mp = map[tx][ty];
			if (mp == NULL)
				continue;
			if      (unitmode && mp->unt != NOUNITS)
				des = units[mp->unt]->des;
			else if (shipmode && mp->shp != NOSHIPS)
				des = ships[mp->shp]->des;
			else if (planemode && mp->pln != NOPLANES)
				des = planes[mp->pln]->des;
			else if (sflg && mp->surv)
				des = mp->surv;
			else
				des = mp->des;
			mvaddch(y - starty, x - startx - 1,
				(mp->mark) ? mp->mark : ' ');
			if (des) {
				if (mp->own == 2)
					standout();
				mvaddch(y - starty, x - startx, des);
				if (mp->own == 2)
					standend();
			}
		}
}
