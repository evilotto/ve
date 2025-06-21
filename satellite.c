
#include	"ve.h"

/* satellite.c - satellite routines */

void
satellite(fp)
	FILE   *fp;
{
	int     xp, yp;
	float   rp;
	int     xr, yr;
	char    radarmap[RADARSIZE][RADARSIZE];
	char    buf[80];

	while (fgets(buf, 80, fp)) {
		if (strncmp(buf, "Satellite ", 10) != 0)
			continue;

		switch (buf[10]) {
		case 'S':		    /* Spy Report */
			if (!isamap(fp))
				continue;
			if (!readradarlines(fp, &xp, &yp, &rp))
				continue;

			/* Do a sanity check */
			if (fgets(buf, 80, fp) == NULL)
				return;
			if (strncmp(buf, "Satellite sector report", 23) != 0)
				return;

			/* Toss the date and header line */
			if (fgets(buf, 80, fp) == NULL)
				return;
			if (fgets(buf, 80, fp) == NULL)
				return;

			/* Read the spy data */
			while (fgets(buf, 80, fp) && buf[4] == ',')
				getspydata(buf);

			/* The next line should be blank */
			if (getc(fp) != '\n')	/* bag it */
				return;

			/* Do another sanity check */
			if (fgets(buf, 80, fp) == NULL)
				return;
			if (strncmp(buf, "Satellite ship report", 21) != 0)
				return;

			/* Toss the date and header line */
			if (fgets(buf, 80, fp) == NULL)
				return;
			if (fgets(buf, 80, fp) == NULL)
				return;

			/* Read the ship data */
			while (fgets(buf, 80, fp) && buf[31] == ',') {
				getshipdata(buf);
				buf[31] = ' ';
			}

			/* Do another sanity check */
			if (fgets(buf, 80, fp) == NULL)
				return;
			if (fgets(buf, 80, fp) == NULL)
				return;
			if (strncmp(buf, "Satellite radar report", 22) != 0)
				return;
			/* Read the radar data */
			presetradarmap(radarmap);
			if (!readradarscan(fp, radarmap, &xr, &yr))
				continue;
			editradarscan(radarmap, &xr, &yr, &xp, &yp);


			/* That's all for now */
			break;

		case 'M':		    /* Map Report */
			/*
			 * Sort of like radarscan() but
			 * different
			 */
			while (isamap(fp)) {
				if (!readradarlines(fp, &xp, &yp, &rp))
					continue;
				if (fgets(buf, 80, fp) == NULL)
					return;
				if (rp < 0.6)
					continue;
				presetradarmap(radarmap);
				if (!readradarscan(fp, radarmap, &xr, &yr))
					continue;
				editradarscan(radarmap, &xr, &yr, &xp, &yp);
			}

			break;

		default:		    /* Error */
			break;
		}
	}
}


/* isamap - return true if input is the start of a satellite map  */
/*	landsat 2 #158 at -33,-25 efficiency 100%, max range 13.7 */

int
isamap(fp)
	FILE   *fp;
{
	char    c, *sp;

	if ((c = getc(fp)) == EOF)
		return (0);

	/* Skip the satelite name */
	while (c != EOF && c != '\n' && c != '#')
		c = getc(fp);

	if (c != '#')			    /* This ain't a
					     * satellite map */
		return (0);

	if (!isdigit(getc(fp)))
		return (0);

	/* Skip the satellite number */
	while (isdigit(getc(fp)));

	/* Check for "at " */
	for (sp = "at "; *sp; ++sp)
		if (getc(fp) != *sp)
			return (0);

	return (1);
}


void
getspydata(buf)				    /* The guts of spy.c */
	char   *buf;
{
	register int x, y;
	register int nx, ny;
	Sector *mp;
	Value  *vp;
	register int i;

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


void
getshipdata(buf)			    /* The guts of ship() */
	char   *buf;
{
	register int i;
	char    des;
	int     number, own;
	int     tx, ty;
	Sector *mp;
	Ship   *sp;
	Value  *vp;

	sscanf(buf, "%d %d", &own, &number);

	if ((i = locship(number)) == NOSHIPS) {
		i = shipcount;
		if (++shipcount >= MAXSHIPS) {
			fputs("Ship vector overflow!\n", stderr);
			(void) fflush(stderr);
			exit(1);
		}
		sp = ships[i] = newship();
	} else {
		sp = ships[i];
		if ((mp = map[xoffset(sp->x)][yoffset(sp->y)]) &&
		    mp->shp != NOSHIPS)
			mp->shp = NOSHIPS;
	}

	if (sp->number != number) {
		strncpy(sp->type, &buf[10], 15);
		sp->type[15] = '\0';
		des = buf[10];
		if (des > 'Z')
			des -= ('a' - 'A');
		sp->des = des;
		sp->number = number;
		sp->x = atoi(&buf[27]);
		sp->y = atoi(&buf[32]);
		sp->fleet = ' ';
	}
	if (sp->vp == NULL) {
		vp = sp->vp = newval();
		vp->val[COU] = own;
		vp->val[EFF] = atoi(&buf[37]);
	}
	vp = sp->vp;
	mp = map[tx = xoffset(sp->x)][ty = yoffset(sp->y)];
	if (mp == NULL)
		map[tx][ty] = mp = newmap();
	if (mp->shp == NOSHIPS)
		mp->shp = i;
	if (mp->own == 0)
		mp->own = 1;
}
