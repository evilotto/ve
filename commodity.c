/* commodity.c - commodity parsing etc. */

# include	"ve.h"

/*
 * commodities - Read and process commodities file.
 */
commodities(fp)
	FILE   *fp;
{
	register int x, y;
	Sector *mp;
	Value  *vp;
	char    buf[BUFSIZ];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[4] != ',')
			continue;
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
		sscanf(&buf[12], "%c%c%c%c%c%c%c%c%c%c",
		       &vp->del[SH], &vp->del[GUN], &vp->del[PET],
		       &vp->del[IRON], &vp->del[DUST], &vp->del[BAR],
		       &vp->del[CRU], &vp->del[LCM], &vp->del[HCM],
		       &vp->del[RAD]);
		sscanf(&buf[23], "%c%c%c%c%c%c%c%c%c%c",
		       &vp->dist[SH], &vp->dist[GUN], &vp->dist[PET],
		       &vp->dist[IRON], &vp->dist[DUST], &vp->dist[BAR],
		       &vp->dist[CRU], &vp->dist[LCM], &vp->dist[HCM],
		       &vp->dist[RAD]);
		sscanf(&buf[33], "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd",
		       &vp->val[SH], &vp->val[GUN], &vp->val[PET],
		       &vp->val[IRON], &vp->val[DUST], &vp->val[BAR],
		       &vp->val[CRU], &vp->val[LCM], &vp->val[HCM],
		       &vp->val[RAD]);
	}
}
