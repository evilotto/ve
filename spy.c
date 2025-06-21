/* spy.c - routines for delaing with spy reports etc. */

# include	"ve.h"

/*
 * spy - Read and process spy reports.
 */
void
spy(fp)
FILE   *fp;
{
	register int x, y;
	char    des;
	register int number;
	register int i;
	int     tx, ty;
	int     j, cnum;
	char    buf[BUFSIZ];
	char    sname[16];
	register int nx, ny;
	Sector *mp;
	Value  *vp;
	Unit *up;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[4] == ','){
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
			mp->own = 2;
			vp->val[COU] = atoi(&buf[15]);
			if (buf[14] != 'N') {
				vp->val[DES] = merge(buf[12], nx, ny, TRUE);
				vp->val[EFF] = atoi(&buf[20]);
				sscanf(&buf[24], "%hd%hd%hd%hd%hd%hd%hd",
				    &vp->val[CIV], &vp->val[MIL], &vp->val[SH],
				    &vp->val[GUN], &vp->val[IRON], &vp->val[PET],
				    &vp->val[FOOD]);
			}
			vp->val[CHKPT] = ' ';
			for (i = 0; i < DELSIZE; i++)
				vp->del[i] = '.';
			for (i = 0; i < DISTSIZE; i++)
				vp->dist[i] = '.';
		}

		if (buf[4] == 's'){
			for (j = 0, i = 34; j < 15 && buf[i] != '#'; ++i, ++j)
				sname[j] = buf[i];
			sname[j] = '\0';
			des = *sname;
			if (des > 'Z')
				des -= ('a' - 'A');
			for (; buf[i] != '#'; i++); /* find unit number */
			number = atoi(&buf[++i]);
			if ((j = locunit(number)) == NOUNITS) {
				j = unitcount;
				if (++unitcount >= MAXUNITS)
					error(1, "Unit vector overflow");
				up = units[j] = newunit();
				up->des = des;
				up->number = number;
				strcpy(up->type, sname);
				up->type[15] = '\0';
				up->x = atoi(&buf[27]);
				for (i = 28; buf[i] != ','; i++)
					continue;
				up->y = atoi(&buf[++i]);
				up->army = ' ';
				if (up->vp == NULL)
					up->vp = newval();
				for ( i = 50; buf[i] != ' ';i++);
				up->vp->val[EFF] = atoi(&buf[i++]);
				i++;
				for ( ;buf[i] != ' ' ; i++);
				up->vp->val[TECH] = atoi(&buf[i]);
				tx = xoffset(up->x);
				ty = yoffset(up->y);
				maxxy(tx, ty);

				mp = map[tx][ty];
				if (mp == NULL)
					map[tx][ty] = mp = newmap();
				if (mp->unt == NOUNITS)
					mp->unt = j;
				if (mp->own == 0)
					mp->own = 1;
				else {
					cnum = mp->vp->val[COU];
					up->vp->val[COU] = cnum;
				}
			}
		}
	}
}
