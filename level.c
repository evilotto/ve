/* level.c - threshold level routines */

#include "ve.h"

Level  *newlev();

level(fp)
	FILE   *fp;
{
	register int x, y;
	register int nx, ny;
	char    buf[BUFSIZ];
	Sector *mp;
	Level  *lp;
	Value  *vp;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[3] != ',')
			continue;
		x = atoi(buf);
		y = atoi(&buf[4]);
		nx = xoffset(x);
		ny = yoffset(y);
		maxxy(nx, ny);
		mp = map[nx][ny];
		if (mp == NULL)
			map[nx][ny] = mp = newmap();
		if (mp->vp == NULL)
			mp->vp = newval();
		vp = mp->vp;
		vp->val[DES] = merge(buf[8], nx, ny, TRUE);
		lp = mp->lp;
		if (lp == NULL)
			mp->lp = lp = newlev();
		(void) strncpy(lp->path, &buf[10], 10);
		lp->dx = atoi(&buf[10]);
		lp->dy = atoi(&buf[14]);
		sscanf(&buf[19], "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd",
		       &lp->thresh[CIV], &lp->thresh[MIL],
		       &lp->thresh[UW], &lp->thresh[FOOD],
		       &lp->thresh[SH], &lp->thresh[GUN],
		       &lp->thresh[PET], &lp->thresh[IRON],
		       &lp->thresh[DUST], &lp->thresh[BAR],
		       &lp->thresh[CRU], &lp->thresh[LCM],
		       &lp->thresh[HCM], &lp->thresh[RAD]);
		vp->dist[CIV] = distval(lp->thresh[CIV]);
		vp->dist[MIL] = distval(lp->thresh[MIL]);
		vp->dist[UW] = distval(lp->thresh[UW]);
		vp->dist[FOOD] = distval(lp->thresh[FOOD]);
		vp->dist[SH] = distval(lp->thresh[SH]);
		vp->dist[GUN] = distval(lp->thresh[GUN]);
		vp->dist[PET] = distval(lp->thresh[PET]);
		vp->dist[IRON] = distval(lp->thresh[IRON]);
		vp->dist[DUST] = distval(lp->thresh[DUST]);
		vp->dist[BAR] = distval(lp->thresh[BAR]);
		vp->dist[CRU] = distval(lp->thresh[CRU]);
		vp->dist[LCM] = distval(lp->thresh[LCM]);
		vp->dist[HCM] = distval(lp->thresh[HCM]);
		vp->dist[RAD] = distval(lp->thresh[RAD]);
	}
}

Level  *
newlev()
{
	Level  *lp;

	lp = (Level *) calloc(1, sizeof(*lp));
	if (lp == NULL)
		error(1, "Out of memory in newlev");
	return lp;
}
