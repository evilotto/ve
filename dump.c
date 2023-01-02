/* dump.c - dump all ve knows about world routines */

# include	"ve.h"
#ifdef VMS
# include types.h
#else
# include	<sys/types.h>
#endif

void    prdate();
void    revert();
char   *splur();
/*
 *	dump - will dump out all the information about the known world
 *	into two files .dump and .map. These files should look as
 *	much like empire report files as possible
 */

FILE   *oldfp;
FILE   *curfp;

dump()
{
	extern void dump_status();
/*	Empire 1.0 strongly urges the use of 'dump'.  'census' and
	'commodity' are not up-to-date.  And 'dump' contains all
	the info needed. */
/*	if (divert(censusfile)) {
 *		dump_status("Dumping census to %s...", censusfile);
 *		cens();
 *		revert();
 *	}
 *	if (divert(commodityfile)) {
 *		dump_status("Dumping commodities to %s...", commodityfile);
 *		comm();
 *		revert();
 *	}
 */
	curfp = stdout;
	if (divert(mapfile)) {
		dump_status("Dumping map to %s...", mapfile);
		do_map();
		revert();
	}
	if (divert(dumpfile)) {
		dump_status("Dumping sector info to %s...", dumpfile);
		do_dump();
		revert();
	}
	dump_status("...done");
}

/*VARARGS1*/
void
dump_status(va_alist)
	va_dcl
{
	va_list ap;
	char *fmt, buf[ 1024 ];

	va_start(ap);
	fmt = va_arg(ap, char *);
	(void)uprintf(buf, fmt, &ap);
	va_end(ap);
	printAtBot(buf);
	refresh();
}

int
divert(name)
	char   *name;
{
	FILE   *fopen();

	oldfp = curfp;

	if ((curfp = fopen(name, "w")) == NULL) {
		putline("Cannot open %s", name);
		revert();
		return (FALSE);
	}
	return (TRUE);
}

void
revert()
{
	(void) fclose(curfp);
	curfp = oldfp;
}
map_rel(x)
	int     x;
{
	return (x - (MAPSIZE / 2));
}
cens()
{

	Sector *this_sector;
	Value  *sector_values;
	short  *theitems;
	int     i, j;
	int     nsect;

	fprintf(curfp, "cen according to ve\n");

	prdate();

	nsect = 0;
	for (i = 0; i < MAPSIZE; i++)
		for (j = 0; j < MAPSIZE; j++) {
			if ((this_sector = map[j][i]) == NULL)
				/*
				 * we know nothing about
				 * sector
				 */
				continue;
			sector_values = this_sector->vp;
			if (sector_values == NULL)
				continue;
			if (nsect++ == 0) {
				fprintf(curfp, "CENSUS              del dst\n");
				fprintf(curfp, "  sect      eff mob uf uf *  civ  mil   uw food min gold fert oil uran work ter\n");
	  }
			theitems = sector_values->val;
			fprintf(curfp, "%3d,%-3d", map_rel(j), map_rel(i));
			fprintf(curfp, " %c", this_sector->des);
			if (this_sector->own != 1) {
				if (sector_values->val[COU] >= 0 &&
				    sector_values->val[COU] < 26)
					fprintf(curfp, "%c", sector_values->val[COU] + 'A');
				else
					fprintf(curfp, "A");
			} else
				fprintf(curfp, "%c", this_sector->sdes);
			fprintf(curfp, "%4d%%", theitems[EFF]);
			fprintf(curfp, "%4d", theitems[MOB]);
			fprintf(curfp, " %c", sector_values->del[UW]);
			fprintf(curfp, "%c", sector_values->del[FOOD]);
			fprintf(curfp, " %c", sector_values->dist[UW]);
			fprintf(curfp, "%c ", sector_values->dist[FOOD]);
			fprintf(curfp, "%c", theitems[CHKPT]);
			fprintf(curfp, "%5d", theitems[CIV]);
			fprintf(curfp, "%5d", theitems[MIL]);
			fprintf(curfp, "%5d", theitems[UW]);
			fprintf(curfp, "%5d", theitems[FOOD]);
			fprintf(curfp, "%4d", theitems[MIN]);
			fprintf(curfp, "%5d", theitems[GOLD]);
			fprintf(curfp, "%5d", theitems[FERT]);
			fprintf(curfp, "%4d", theitems[OIL]);
			fprintf(curfp, "%5d", theitems[URAN]);
			fprintf(curfp, "%4d%%", theitems[WORK]);
			fprintf(curfp, "%4d%\n", theitems[TERR]);
		}
	fprintf(curfp, "    %d sector%s\n", nsect, splur(nsect));
	return;
}
comm()
{
	Sector *this_sector;
	Value  *sector_values;
	short  *theitems;
	char   *dels;
	char   *dists;
	int     i, j;
	int     nsect;

	fprintf(curfp, "%s\n", "com according to ve");

	prdate();

	nsect = 0;
	for (i = 0; i < MAPSIZE; i++)
		for (j = 0; j < MAPSIZE; j++) {
			if ((this_sector = map[j][i]) == NULL)
				/*
				 * we know nothing about
				 * this sector
				 */
				continue;
			sector_values = this_sector->vp;
			if (sector_values == NULL)
				continue;
			if (nsect++ == 0) {
				fprintf(curfp, "COMMODITIES deliver-- distribute\n");
				fprintf(curfp, " sect     sgpidbclhr sgpidbclhr  sh  gun  pet iron dust  bar  cru  lcm  hcm rad\n");
			}
			theitems = sector_values->val;
			dels = sector_values->del;
			dists = sector_values->dist;
			fprintf(curfp, "%3d,%-3d", map_rel(j), map_rel(i));
			fprintf(curfp, " %c", this_sector->des);
			fprintf(curfp, " %c", dels[SH]);
			fprintf(curfp, "%c", dels[GUN]);
			fprintf(curfp, "%c", dels[PET]);
			fprintf(curfp, "%c", dels[IRON]);
			fprintf(curfp, "%c", dels[DUST]);
			fprintf(curfp, "%c", dels[BAR]);
			fprintf(curfp, "%c", dels[CRU]);
			fprintf(curfp, "%c", dels[LCM]);
			fprintf(curfp, "%c", dels[HCM]);
			fprintf(curfp, "%c", dels[RAD]);
			fprintf(curfp, " %c", dists[SH]);
			fprintf(curfp, "%c", dists[GUN]);
			fprintf(curfp, "%c", dists[PET]);
			fprintf(curfp, "%c", dists[IRON]);
			fprintf(curfp, "%c", dists[DUST]);
			fprintf(curfp, "%c", dists[BAR]);
			fprintf(curfp, "%c", dists[CRU]);
			fprintf(curfp, "%c", dists[LCM]);
			fprintf(curfp, "%c", dists[HCM]);
			fprintf(curfp, "%c", dists[RAD]);
			fprintf(curfp, "%4d", theitems[SH]);
			fprintf(curfp, "%5d", theitems[GUN]);
			fprintf(curfp, "%5d", theitems[PET]);
			fprintf(curfp, "%5d", theitems[IRON]);
			fprintf(curfp, "%5d", theitems[DUST]);
			fprintf(curfp, "%5d", theitems[BAR]);
			fprintf(curfp, "%5d", theitems[CRU]);
			fprintf(curfp, "%5d", theitems[LCM]);
			fprintf(curfp, "%5d", theitems[HCM]);
			fprintf(curfp, "%4d", theitems[RAD]);
			putc('\n', curfp);
		}
	fprintf(curfp, "    %d sector%s\n", nsect, splur(nsect));
	return;
}

do_dump()
{
	Sector *this_sector;
	Value  *sector_values;
	short  *theitems;
	char   *del;
	short   *cutoff;
	short   *thresh;
	int     i, j;
	int     nsect;

	fprintf(curfp, "dump info according to ve\n");

	prdate();

	nsect = 0;
	for (i = 0; i < MAPSIZE; i++) {
		for (j = 0; j < MAPSIZE; j++) {
			if ((this_sector = map[j][i]) == NULL)
				/*
				 * we know nothing about
				 * sector
				 */
				continue;
			sector_values = this_sector->vp;
			if (sector_values == NULL)
				continue;
			if (nsect++ == 0) {
				fprintf(curfp, "DUMP SECTOR\n");
				fprintf(curfp, "\n");
	  		}
			theitems = sector_values->val;
			del = sector_values->del;
#ifdef OWNER
			fprintf(curfp, "%d ", sector_values->val[COU]);
#endif
			fprintf(curfp, "%d %d", map_rel(j), map_rel(i));
			fprintf(curfp, " %c", this_sector->des);
#ifndef OWNER
			if (this_sector->own != 1) {
				if (sector_values->val[COU] >= 0 &&
				    sector_values->val[COU] < 26)
					fprintf(curfp, " %c",
					    sector_values->val[COU] + 'A');
				else
					fprintf(curfp, " A");
			} else 
#endif
			if (this_sector->sdes == ' ') {
				putc(' ', curfp);
				putc('_', curfp);
			} else
				fprintf(curfp, " %c", this_sector->sdes);
			fprintf(curfp, " %d", theitems[EFF]);
			fprintf(curfp, " %d", theitems[MOB]);
			if (theitems[CHKPT] == ' ') {
				putc(' ', curfp);
				putc('.', curfp);
			} else	
				fprintf(curfp, " %c", theitems[CHKPT]);
			fprintf(curfp, " %d", theitems[MIN]);
			fprintf(curfp, " %d", theitems[GOLD]);
			fprintf(curfp, " %d", theitems[FERT]);
			fprintf(curfp, " %d", theitems[OIL]);
			fprintf(curfp, " %d", theitems[URAN]);
			fprintf(curfp, " %d", theitems[WORK]);
			fprintf(curfp, " %d", theitems[AVAIL]);
			fprintf(curfp, " %d", theitems[TERR]);

			fprintf(curfp, " %d", theitems[CIV]);
			fprintf(curfp, " %d", theitems[MIL]);
			fprintf(curfp, " %d", theitems[UW]);
			fprintf(curfp, " %d", theitems[FOOD]);
			fprintf(curfp, " %d", theitems[SH]);
			fprintf(curfp, " %d", theitems[GUN]);
			fprintf(curfp, " %d", theitems[PET]);
			fprintf(curfp, " %d", theitems[IRON]);
			fprintf(curfp, " %d", theitems[DUST]);
			fprintf(curfp, " %d", theitems[BAR]);
			fprintf(curfp, " %d", theitems[CRU]);
			fprintf(curfp, " %d", theitems[LCM]);
			fprintf(curfp, " %d", theitems[HCM]);
			fprintf(curfp, " %d", theitems[RAD]);

			fprintf(curfp, " %c", del[UW]);
			fprintf(curfp, " %c", del[FOOD]);
			fprintf(curfp, " %c", del[SH]);
			fprintf(curfp, " %c", del[GUN]);
			fprintf(curfp, " %c", del[PET]);
			fprintf(curfp, " %c", del[IRON]);
			fprintf(curfp, " %c", del[DUST]);
			fprintf(curfp, " %c", del[BAR]);
			fprintf(curfp, " %c", del[CRU]);
			fprintf(curfp, " %c", del[LCM]);
			fprintf(curfp, " %c", del[HCM]);
			fprintf(curfp, " %c", del[RAD]);

			if (this_sector->lp == NULL) {
				fprintf(curfp, " 0 0 0 0 0 0 0 0 0 0 0 0");
				fprintf(curfp, " _ 0 0");
				fprintf(curfp, " 0 0 0 0 0 0 0 0 0 0 0 0\n");
				continue;
			}

			cutoff = this_sector->lp->cutoff;
			thresh = this_sector->lp->thresh;

			fprintf(curfp, " %d", cutoff[UW]);
			fprintf(curfp, " %d", cutoff[FOOD]);
			fprintf(curfp, " %d", cutoff[SH]);
			fprintf(curfp, " %d", cutoff[GUN]);
			fprintf(curfp, " %d", cutoff[PET]);
			fprintf(curfp, " %d", cutoff[IRON]);
			fprintf(curfp, " %d", cutoff[DUST]);
			fprintf(curfp, " %d", cutoff[BAR]);
			fprintf(curfp, " %d", cutoff[CRU]);
			fprintf(curfp, " %d", cutoff[LCM]);
			fprintf(curfp, " %d", cutoff[HCM]);
			fprintf(curfp, " %d", cutoff[RAD]);
			if (this_sector->lp->path[0] == ' ') {
				putc( ' ', curfp);
				putc( '_', curfp);
			} else
				fprintf(curfp, " %s",
					this_sector->lp->path);
			fprintf(curfp, " %d %d", this_sector->lp->dx,
				this_sector->lp->dy);

			fprintf(curfp, " %d", thresh[UW]);
			fprintf(curfp, " %d", thresh[FOOD]);
			fprintf(curfp, " %d", thresh[SH]);
			fprintf(curfp, " %d", thresh[GUN]);
			fprintf(curfp, " %d", thresh[PET]);
			fprintf(curfp, " %d", thresh[IRON]);
			fprintf(curfp, " %d", thresh[DUST]);
			fprintf(curfp, " %d", thresh[BAR]);
			fprintf(curfp, " %d", thresh[CRU]);
			fprintf(curfp, " %d", thresh[LCM]);
			fprintf(curfp, " %d", thresh[HCM]);
			fprintf(curfp, " %d", thresh[RAD]);

			putc('\n', curfp);
		}
	}
	fprintf(curfp, "    %d sector%s\n", nsect, splur(nsect));
	return;
}

do_map()
{
	int     cur_x, cur_y, x;
	Sector *this_sector;
	char    des;

	fprintf(curfp, "%s\n", "map according to ve");

	border(map_rel(min_x), map_rel(max_x) + 1, 1, "     ", "");

	for (cur_y = min_y; cur_y <= max_y; cur_y++) {
		fprintf(curfp, "%4d ", map_rel(cur_y));
		if (cur_y % 2) {
			if (min_x % 2)
				x = min_x;
			else {
				x = min_x + 1;
				putc(' ', curfp);
			}
		} else {
			if (min_x % 2) {
				x = min_x + 1;
				putc(' ', curfp);
			} else
				x = min_x;
		}

		for (cur_x = x; cur_x <= max_x; cur_x += 2) {
			if ((this_sector = map[cur_x][cur_y]) == NULL)
				des = ' ';
			else
				des = this_sector->des;
			fprintf(curfp, "%c ", des);
		}
		if (((cur_y % 2) && !(max_x % 2)) || (!(cur_y % 2) && (max_x % 2)))
			fprintf(curfp, " ");
		fprintf(curfp, "%-4d\n", map_rel(cur_y));
	}
	border(map_rel(min_x), map_rel(max_x) + 1, 1, "     ", "");

}
border(xmin, xmax, inc, indent, sep)
	int     xmin;
	int     xmax;
	int     inc;
	char   *indent;
	char   *sep;
{
	register int r11;
	register int r10;

	if (xmin < -99 || xmin > 99 || xmax < -99 || xmax > 99) {
		/*
		 * hundreds
		 */
		fprintf(curfp, "%s", indent);
		for (r11 = xmin; r11 != xmax; r11 += inc) {
			fprintf(curfp, "%s", sep);
			if (r11 < 0 && r11 > -100) {
				putc('-', curfp);
				continue;
			}
			r10 = (r11 < 0 ? -r11 : r11) / 100;
			fprintf(curfp, "%d", r10 % 10);
		}
		putc('\n', curfp);
	}
	/*
	 * tens
	 */
	fprintf(curfp, "%s", indent);
	for (r11 = xmin; r11 != xmax; r11 += inc) {
		fprintf(curfp, "%s", sep);
		if (r11 < 0 && r11 > -10) {
			putc('-', curfp);
			continue;
		}
		r10 = (r11 < 0 ? -r11 : r11) / 10;
		fprintf(curfp, "%d", r10 % 10);
	}
	putc('\n', curfp);
	fprintf(curfp, "%s", indent);
	for (r11 = xmin; r11 != xmax; r11 += inc) {
		r10 = (r11 < 0 ? -r11 : r11);
		fprintf(curfp, "%s%d", sep, r10 % 10);
	}
	putc('\n', curfp);
}



char   *
splur(n)
	int     n;
{
	return (n == 1 ? "" : "s");
}

void
prdate()
{
	long    now, time();
	extern char *ctime();

	(void) time(&now);
	fprintf(curfp, "%s", ctime(&now));
}
