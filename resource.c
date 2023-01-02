/* resource.c - resource parsing etc. */

# include	"ve.h"

/*
 * resource - Read and process sector resource file.
 */
resource(fp)
	FILE   *fp;
{
	register int x, y;
	Sector *mp;
	Value  *vp;
	char    buf[BUFSIZ];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[4] != ',') {
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
		if (mp->own == 0)
			mp->own = 1;
		vp->val[DES] = merge(buf[10], x, y, TRUE);
		vp->val[EFF] = atoi(&buf[13]);
		sscanf(&buf[19], "%hd%hd%hd%hd%hd",
		       &vp->val[MIN], &vp->val[GOLD], &vp->val[FERT],
		       &vp->val[OIL], &vp->val[URAN]);
		if (buf[42])
			vp->val[TERR] = atoi(&buf[42]);
		else
			vp->val[TERR] = 0;
	}
}
