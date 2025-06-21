/* cutoff.c - delivery threshold level routines */

#include "ve.h"

Level  *newlev();

void
cutoff(fp)
	FILE   *fp;
{
	register int x, y;
	register int nx, ny;
	char    buf[BUFSIZ];
	Sector *mp;
	Level  *lp;
	Value  *vp;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[4] != ',')
			continue;
		x = atoi(buf);
		y = atoi(&buf[5]);
		nx = xoffset(x);
		ny = yoffset(y);
		maxxy(nx, ny);
		mp = map[nx][ny];
		if (mp == NULL)
			map[nx][ny] = mp = newmap();
		if (mp->vp == NULL)
			mp->vp = newval();
		vp = mp->vp;
		vp->val[DES] = merge(buf[10], nx, ny, TRUE);
		lp = mp->lp;
		if (lp == NULL)
			mp->lp = lp = newlev();
		sscanf(&buf[12], "%c%c%c%c%c%c%c%c%c%c%c%c",
		       &vp->del[UW], &vp->del[FOOD],
		       &vp->del[SH], &vp->del[GUN], &vp->del[PET],
		       &vp->del[IRON], &vp->del[DUST], &vp->del[BAR],
		       &vp->del[CRU], &vp->del[LCM], &vp->del[HCM],
		       &vp->del[RAD]);
		sscanf(&buf[24], "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd",
		       &lp->cutoff[UW], &lp->cutoff[FOOD],
		       &lp->cutoff[SH], &lp->cutoff[GUN],
		       &lp->cutoff[PET], &lp->cutoff[IRON],
		       &lp->cutoff[DUST], &lp->cutoff[BAR],
		       &lp->cutoff[CRU], &lp->cutoff[LCM],
		       &lp->cutoff[HCM], &lp->cutoff[RAD]);
	}
}
