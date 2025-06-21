/* census.c - census routines */

# include "ve.h"

/*
 * Special file processing functions
 *
 * These routines parse the information from the "special" file,
 *
 * Hacked in by Steven Grady -- grady@postgres.berkeley.edu
 */

/*
 * special - Read and process special file.
 */
void
special(fp)
	FILE   *fp;
{
	register int x, y;
	Sector *mp;
	Value  *vp;
	char    buf[BUFSIZ];
	int     val;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		x = xoffset(atoi(buf));
		y = yoffset(atoi(&buf[4]));
		val = atoi(&buf[9]);
		maxxy(x, y);
		mp = map[x][y];
		if (mp == NULL)
			map[x][y] = mp = newmap();
		if (mp->vp == NULL)
			mp->vp = newval();
		vp = mp->vp;
		mp->own = 1;
		vp->val[SPECIAL] = val;
		vp->val[COU] = -1;
	}
}
