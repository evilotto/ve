/* lookout.c - lookout routines */

# include "ve.h"

Item    stypes[] = {
	"agri", 4, 'a',
	"bank", 4, 'b',
	"capi", 4, 'c',
	"defe", 4, 'd',
	"enli", 4, 'e',
	"fort", 4, 'f',
	"gold", 4, 'g',
	"harb", 4, 'h',
	"shel", 4, 'i',
	"ligh", 4, 'j',
	"heav", 4, 'k',
	"libr", 4, 'l',
	"mine", 4, 'm',
	"nucl", 4, 'n',
	"oil", 3, 'o',
	"park", 4, 'p',
	"rese", 4, 'r',
	"sanc", 4, 's',
	"tech", 4, 't',
	"uran", 4, 'u',
	"ware", 4, 'w',
	"airf", 4, '*',
	"rada", 4, ')',
	"high", 4, '+',
	"bridge h", 8, '#',
	"bridge s", 8, '=',
	"refi", 4, '%',
	"wast", 4, '/',
	"moun", 4, '^',
	"wild", 4, '-',
	"head", 4, '!',
	"", 0, -1,
};

extern char *findblank();
extern char *findchar();

void
lookout(fp)
	FILE   *fp;
{
	int     i, j;
	char    buf[BUFSIZ];
	char    cname[16], des;
	char   *p;
	Item   *ip;
	Value  *vp;
	Sector *mp;
	Ship   *sp;
	Unit   *up;
	char   uname[16];
	register int number;
	int     cnum, eff, x, y, tx, ty, civ, mil;

	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (strncmp(buf, "Sonar contact", 13) == 0) {
			/* A special case for Sonar */
			i = shipcount;
			if (++shipcount >= MAXSHIPS)
				error(1, "Ship vector overflow");
			sp = ships[i] = newship();
			sp->des = 'S';
			sp->number = UNKNOWN;
			strcpy(sp->type, "Sonar Echo");
			p = buf + 16;	    /* Skip to the coords */
			sp->x = atoi(p);
			for (; *p != ','; p++);
			sp->y = atoi(++p);
			sp->fleet = ' ';
			x = xoffset(sp->x);
			y = yoffset(sp->y);
			maxxy(x, y);

			mp = map[x][y];
			if (mp == NULL)
				map[x][y] = mp = newmap();
			if (mp->shp == NOSHIPS)
				mp->shp = i;
			if (mp->own == 0)
				mp->own = 1;
			continue;
		} else {
			strncpy(cname, buf, 16);
			for (i = 0, p = buf; *p && *p != '('; ++p, ++i);
			if (*p != '(')
				continue;
			if (*++p != '#')
				continue;
			cnum = atoi(++p);
			if (CountryNames[cnum][0] == '\0') {
				if (i > 15)
					i = 15;
				cname[i] = '\0';
				strcpy(CountryNames[cnum], cname);
			}
		}
		p = findblank(p);
		p = findchar(p);
		if ((index(p, '%')) == NULL) {	/* Must be a ship */
			shiplook(p, cnum);
			continue;
		}
		for (ip = stypes; ip->len; ip++)	/* Find sector type */
			if (strncmp(p, ip->nm, ip->len) == 0)
				break;
		if (!ip->len)
		  continue;

		p = findblank(p);
		p = findchar(p);
		eff = atoi(p);
		if ((p = index(p, '%')) == NULL)
			continue;
		p++;
		p = findchar(p);
		p = findblank(p);
		p = findchar(p);
		if (strncmp(p, "with", 4) == 0) {
			while (*p && !isdigit(*p))
				p++;
			if (*p == '\0')
				continue;
			civ = atoi(p);
		} else
			civ = 0;

		/* New section to compute and save mil data for sectors */

		p = findblank(p);
		p = findchar(p);
		p = findblank(p);
		p = findchar(p);
		if (strncmp(p, "with", 4) == 0) {
			while (*p && !isdigit(*p))
				p++;
			if (*p == '\0')
				continue;
			mil = atoi(p);
		} else
			mil = 0;

		if ((p = index(p, '@')) == NULL)
			continue;
		if (sscanf(p, "@%d, %d", &x, &y) != 2)
			continue;
		tx = xoffset(x);
		ty = yoffset(y);
		mp = map[tx][ty];
		if (mp == NULL)
			mp = map[tx][ty] = newmap();
		if (mp->vp == NULL)
			mp->vp = newval();
		vp = mp->vp;
		if (mp->own && vp->val[COU] == -1)
			continue;
		mp->own = 2;
		vp->val[COU] = cnum;
		vp->val[DES] = merge(ip->value, tx, ty, TRUE);
		vp->val[CIV] = civ;
		vp->val[EFF] = eff;
		vp->val[MIL] = mil;
		vp->val[CHKPT] = ' ';
		for (i = 0; i < DELSIZE; i++)
			vp->del[i] = '.';
		for (i = 0; i < DISTSIZE; i++)
			vp->dist[i] = '.';
	}
}
void
shiplook(p, cnum)
	char   *p;
	int     cnum;
{
	char    des;
	char    sname[16];
	register int number;
	int     tx, ty;
	int     j;
	Sector *mp;
	Ship   *sp;
	Unit   *up;

	for (j = 0; p[j] != '@'; j++)
	  ;
	if (p[j+1] == ' ') {
	  for (j = 0; *p != '#' && j < 15; ++p, ++j)
	    sname[j] = *p;
	  sname[j] = '\0';
	  des = *sname;
	  if (des > 'Z')
	    des -= ('a' - 'A');
	  
	  for (; *p != '#'; p++);		    /* find ship number */
	  number = atoi(++p);
	  if ((j = locship(number)) == NOSHIPS) {
	    j = shipcount;
	    if (++shipcount >= MAXSHIPS)
	      error(1, "Ship vector overflow");
	    sp = ships[j] = newship();
	    sp->des = des;
	    sp->number = number;
	    strcpy(sp->type, sname);
	    for (; *p != '@'; p++)
	      continue;	    /* find coordinates */
	    sp->x = atoi(++p);
	    for (; *p != ','; p++)
	      continue;
	    sp->y = atoi(++p);
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
	else { /* It's a unit! */
	  for (j = 0; *p != '#' && j < 15; ++p, ++j)
	    sname[j] = *p;
	  sname[j] = '\0';
	  des = *sname;
	  if (des > 'Z')
	    des -= ('a' - 'A');
	  
	  for (; *p != '#'; p++);		    /* find unit number */
	  number = atoi(++p);
	  if ((j = locunit(number)) == NOUNITS) {
	    j = unitcount;
	    if (++unitcount >= MAXUNITS)
	      error(1, "Unit vector overflow");
	    up = units[j] = newunit();
	    up->des = des;
	    up->number = number;
	    strcpy(up->type, sname);
	    for (; *p != '@'; p++)
	      continue;	    /* find coordinates */
	    up->x = atoi(++p);
	    for (; *p != ','; p++)
	      continue;
	    up->y = atoi(++p);
	    up->army = ' ';
	    if (up->vp == NULL)
	      up->vp = newval();
	    up->vp->val[COU] = cnum;

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
	  }
	}
}
