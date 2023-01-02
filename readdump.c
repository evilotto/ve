/* readdump.c */

# include "ve.h"

/*
 * Dump file processing routine
 *
 * readdump() does not require fixed positions for its data.
 * It does require that one space be placed after all fields.
 *
 * This routine will crash some hosed C compilers.
 * If yours dies, try using another compiler.  If that fails then
 * #define HOSED_CC and you'll have to do without using 'dump'
 * information.
 *
 * Jeff Wallace <jeffw@scam.Berkeley.EDU>
 */

/*
 * readdump - Read and process dump file.
 */
readdump(fp)
	FILE   *fp;
{
	register int x, y;
	Sector *mp;
	Value  *vp;
	Level  *lp;
	char    buf[BUFSIZ];
	int	xtmp, ytmp;
	char	des, sdes, chkpt;
	char	path[11];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (sscanf(buf, "%d%d", &xtmp, &ytmp) != 2)
			continue;
		x = xoffset(xtmp);
		y = yoffset(ytmp);
		maxxy(x, y);
		mp = map[x][y];
		if (mp == NULL)
			map[x][y] = mp = newmap();
		if (mp->vp == NULL)
			mp->vp = newval();
		vp = mp->vp;
		lp = mp->lp;
		if (lp == NULL)
			mp->lp = lp = newlev();
#ifndef HOSED_CC
		if (sscanf(buf, "%*d%*d %c %c %hd%hd %c %hd%hd%hd%hd%hd%hd\
%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd %c %c %c %c %c %c \
%c %c %c %c %c %c%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd %s %hd%hd\
%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd",
			&des, &sdes, &vp->val[EFF], &vp->val[MOB], &chkpt,
			&vp->val[MIN], &vp->val[GOLD], &vp->val[FERT],

			&vp->val[OIL], &vp->val[URAN], &vp->val[WORK],
			&vp->val[AVAIL], &vp->val[TERR],
			&vp->val[CIV], &vp->val[MIL], &vp->val[UW],
			&vp->val[FOOD], &vp->val[SH], &vp->val[GUN],
			&vp->val[PET], &vp->val[IRON], &vp->val[DUST],
			&vp->val[BAR], &vp->val[CRU], &vp->val[LCM],
			&vp->val[HCM], &vp->val[RAD],
			&vp->del[UW], &vp->del[FOOD], &vp->del[SH],
			&vp->del[GUN], &vp->del[PET], &vp->del[IRON],

			&vp->del[DUST], &vp->del[BAR], &vp->del[CRU],
			&vp->del[LCM], &vp->del[HCM], &vp->del[RAD],
			&lp->cutoff[UW], &lp->cutoff[FOOD], &lp->cutoff[SH],
			&lp->cutoff[GUN], &lp->cutoff[PET], &lp->cutoff[IRON],
			&lp->cutoff[DUST], &lp->cutoff[BAR], &lp->cutoff[CRU],
			&lp->cutoff[LCM], &lp->cutoff[HCM], &lp->cutoff[RAD],
			path, &lp->dx, &lp->dy,

			&lp->thresh[UW], &lp->thresh[FOOD],
			&lp->thresh[SH], &lp->thresh[GUN],
			&lp->thresh[PET], &lp->thresh[IRON],
			&lp->thresh[DUST], &lp->thresh[BAR],
			&lp->thresh[CRU], &lp->thresh[LCM],
			&lp->thresh[HCM], &lp->thresh[RAD])==68) 
				{chainsaw = FALSE;
				 break;}
		else 
		  if (sscanf(buf, "%*d%*d %c %c %hd%hd %c %hd%hd%hd%hd%hd%hd\
%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd %c %c %c %c %c %c \
%c %c %c %c %c %c%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd %s %hd%hd\
%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd",
			&des, &sdes, &vp->val[EFF], &vp->val[MOB], &chkpt,
			&vp->off,
			&vp->val[MIN], &vp->val[GOLD], &vp->val[FERT],

			&vp->val[OIL], &vp->val[URAN], &vp->val[WORK],
			&vp->val[AVAIL], &vp->val[TERR],
			&vp->val[CIV], &vp->val[MIL], &vp->val[UW],
			&vp->val[FOOD], &vp->val[SH], &vp->val[GUN],
			&vp->val[PET], &vp->val[IRON], &vp->val[DUST],
			&vp->val[BAR], &vp->val[CRU], &vp->val[LCM],
			&vp->val[HCM], &vp->val[RAD],
			&vp->del[UW], &vp->del[FOOD], &vp->del[SH],
			&vp->del[GUN], &vp->del[PET], &vp->del[IRON],

			&vp->del[DUST], &vp->del[BAR], &vp->del[CRU],
			&vp->del[LCM], &vp->del[HCM], &vp->del[RAD],
			&lp->cutoff[UW], &lp->cutoff[FOOD], &lp->cutoff[SH],
			&lp->cutoff[GUN], &lp->cutoff[PET], &lp->cutoff[IRON],
			&lp->cutoff[DUST], &lp->cutoff[BAR], &lp->cutoff[CRU],
			&lp->cutoff[LCM], &lp->cutoff[HCM], &lp->cutoff[RAD],
			&lp->dx, &lp->dy,

			&lp->thresh[CIV], &lp->thresh[MIL],
			&lp->thresh[UW], &lp->thresh[FOOD],
			&lp->thresh[SH], &lp->thresh[GUN],
			&lp->thresh[PET], &lp->thresh[IRON],
			&lp->thresh[DUST], &lp->thresh[BAR],
			&lp->thresh[CRU], &lp->thresh[LCM],
			&lp->thresh[HCM], &lp->thresh[RAD]) == 70 )
				chainsaw = TRUE;
#ifdef OWNER		
		sscanf(buf, "%d", &vp->val[COU]);
		mp->own = ((vp->val[COU] == cnum) ? 1 : 2);
#else
		mp->own = 1;
		vp->val[COU] = -1;
#endif OWNER
		vp->val[DES] = merge(des, x, y, TRUE);
		if (isupper(sdes)) {
			mp->own = 2;
			vp->val[COU] = sdes - 'A';
			vp->val[SDES] = ' ';
			mp->sdes = ' ';
		} else {
			vp->val[SDES] = sdes;
			mp->sdes = ((sdes == '_') ? ' ' : sdes);
		}
		vp->val[CHKPT] = chkpt;
		if (path[0] == '_') path[0] = ' ';
		(void) strncpy(lp->path, path, 11);
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
#endif HOSED_CC
		}
}
