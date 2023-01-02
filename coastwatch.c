/* coastwatch.c - coastwatching routines */

# include	"ve.h"

coastwatch(fp)
	FILE   *fp;
{
	char    des;
	register int number;
	register int i;
	int     tx, ty;
	int     j, cnum;
	Sector *mp;
	Ship   *sp;
	char    buf[BUFSIZ];
	char    sname[16];

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[14] != '(' || buf[15] != '#')
			continue;

		cnum = atoi(buf + 16);

		if (CountryNames[cnum][0] == '\0')
			strncpy(CountryNames[cnum], buf + 1, 12);
		CountryNames[cnum][12] = '\0';

		for (j = 0, i = 21; j < 15 && buf[i] != '#'; ++i, ++j)
			sname[j] = buf[i];
		sname[j] = '\0';
		des = *sname;
		if (des > 'Z')
			des -= ('a' - 'A');
		for (; buf[i] != '#'; i++); /* find ship number */
		number = atoi(&buf[++i]);
		if ((j = locship(number)) == NOSHIPS) {
			j = shipcount;
			if (++shipcount >= MAXSHIPS)
				error(1, "Ship vector overflow");
			sp = ships[j] = newship();
			sp->des = des;
			sp->number = number;
			strcpy(sp->type, sname);
			sp->type[15] = '\0';
			for (; buf[i] != '@'; i++)
				continue;   /* find coordinates */
			sp->x = atoi(&buf[++i]);
			for (; buf[i] != ','; i++)
				continue;
			sp->y = atoi(&buf[++i]);
			sp->fleet = ' ';
			if (sp->vp == NULL)
				sp->vp = newval();
			sp->vp->val[COU] = cnum;
			tx = xoffset(sp->x);
			ty = yoffset(sp->y);
			maxxy(tx, ty);

			mp = map[tx][ty];
			if (mp == NULL)
				map[tx][ty] = mp = newmap();
			if (mp->shp == NOSHIPS)
				mp->shp = j;
			if (mp->own == 0)
				mp->own = 1;
		}
	}
}
