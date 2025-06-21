/* ve.c - main ve command routines etc. */

/*
 *	ve - visual empire (with due respect to	Peter Langston).
 *
 *	Written	by Matthew Diaz	and Michael Baldwin
 *	Modified by Bill Jones,	Jim Knutson, Ken Montgomery and	Dan Reynolds
 *	Modified for UCSD and UCB versions by Jeff Wallace.
 *
 *	Bit of hackary from Julian Onions <jpo@cs.nott.ac.uk> - largely
 *	Efficiency hacks + profile option & some fiddling.
 *
 *	Usage: ve [-a|c] census	commodity map ship spy radar coastwatch....
 */

# include	"ve.h"
#include <locale.h>
#include <sys/wait.h>
#include "screens.h"
#include "xtn.h"

/* As of empire 1.0, census and commodity dumps have been removed.
   The 'dump' file contains all the needed info.  -- jeffw */
char   *preload[] = {mapfile, dumpfile, 0};

Sector *map[MAPSIZE][MAPSIZE];
Overlay *ovl[MAPSIZE][MAPSIZE];

Unit *units[MAXUNITS];

Ship   *ships[MAXSHIPS];

Plane  *planes[MAXPLANES];


Item    items[] = {
	"civ", 3, CIV,
	"mil", 3, MIL,
	"uw", 2, UW,
	"foo", 3, FOOD,
	"sh", 2, SH,
	"gun", 3, GUN,
	"pet", 3, PET,
	"iro", 3, IRON,
	"dus", 3, DUST,
	"bar", 3, BAR,
	"cru", 3, CRU,
	"lcm", 3, LCM,
	"hcm", 3, HCM,
	"rad", 3, RAD,
	"eff", 3, EFF,
	"mob", 3, MOB,
	"min", 3, MIN,
	"gol", 3, GOLD,
	"fer", 3, FERT,
	"oil", 3, OIL,
	"ura", 3, URAN,
	"wor", 3, WORK,
	"des", 3, DES,
	"cou", 3, COU,
	"dis", 3, DIST,
	"pln", 3, PL,
	"del", 3, DEL,
	"tec", 3, TECH,
	"ch", 2, CHKPT,
	"spe", 3, SPECIAL,
	"sde", 3, SDES,
	"ter", 3, TERR,
	"ava", 3, AVAIL,
	"", 0, -1,
};
/*
 * ve input file function processors
 */
int     census();			    /* census file processor */
int     readmap();			    /* map file	processor */
int     commodities();			    /* commodities file
					     * processor */
int     spy();				    /* spy report processor */
int     radarscan();			    /* naval radar scan
					     * processor */
int     unit();				    /* unit census report
					     * processor */
int     lcargo();			    /* unit lcargo report
					     * processor */
int     ship();				    /* ship census report
					     * processor */
int     cargo();			    /* ship cargo report
					     * processor */
int     plane();			    /* plane report
					     * processor */
int     coastwatch();			    /* coastwatch report
					     * processor */
int     lookout();			    /* lookout report
					     * processor */
int     level();			    /* level report
					     * processor */
int     cutoff();			    /* cutoff report
					     * processor */
int     special();			    /* special list
					     * processor */
int     satellite();			    /* satellite output
					     * processor */
int	readdump(); /* dumpsector report processor */

int	resource(); /* resources report */

Funsw   fsw[] = {
	"cen", census,
	"bma", readmap,
	"map", readmap,
	"com", commodities,
	"spy", spy,
	"rad", radarscan,
	"shi", ship,
	"lan", unit,
	"lca", lcargo,
	"car", cargo,
	"pla", plane,
	"coa", coastwatch,
	"loo", lookout,
	"llo", lookout,
	"cut", cutoff,
	"lev", level,
	"spe", special,
	"sat", satellite,
	"dum", readdump,
	"res", resource,
	"son", lookout,
	0, 0,
};

char   *macros[127];			    /* Macro definitions */
char    peekc;				    /* Lets	you poke a
					     * character */
int     startx = -3000;			    /* Leftmost x-coordinate
					     * of display window */
int     starty = -3000;			    /* Uppermost
					     * y-coordinate of
					     * display window */
int     minx;				    /* Min and max x (used
					     * for map reading)	 */
int     maxx;
int     miny;
int     max_x = -1, min_x = MAPSIZE + 1;
int     max_y = -1, min_y = MAPSIZE + 1;
int     curmark = '>';			    /* Current marking
					     * character */
int     range = 10;			    /* Range for survey */
int     curx, cury;			    /* Current x and y
					     * coordinates in map
					     * array */
int     unitcount = 0;			    /* Total number	of
					     * units in units array	 */
int     shipcount = 0;			    /* Total number	of
					     * ships in ships array	 */
int     planecount = 0;			    /* Total number of
					     * planes is plane array */
int     unitmode = FALSE;                   /* True if in unit display mode */
int     shipmode = FALSE;		    /* True	if in ship
					     * display mode	 */
int     planemode = FALSE;		    /* True if in plane
					     * display mode */
int     surmap = 0;			    /* Nonzero if survey map
					     * should	be displayed */
int     noise = TRUE;			    /* True	if
					     * production messages
					     * to be printed */
int     autowrite = FALSE;		    /* True if we dump file
					     * on quit */
int     autoload = FALSE;		    /* True if we read in
					     * files */
int     incurses = FALSE;		    /* True if curses
					     * running */
int     levelmode = 0;			    /* what we display bt
					     * default */
char    CountryNames[MAXNOC][16];	    /* Table of cnum's ->
					     * Names */

FILE   *inpf;				    /* Input file pointer */
FILE   *outf;				    /* Output file pointer */
char    oname[BUFSIZ];			    /* Output file name */
char    mapfile[BUFSIZ] = SAVMAP;
char    censusfile[BUFSIZ] = SAVCEN;
char    commodityfile[BUFSIZ] = SAVCOM;
char	dumpfile[BUFSIZ] = SAVDUMP;

char    _obuf[BUFSIZ];
char   *invo_name;
int     need_dump = FALSE;

void
load_file(name, quiet)
	char   *name;
	int     quiet;
{
	char    buf[BUFSIZ];
	char   *bp;
	Funsw  *fp;			    /* Pointer to input file
					     * function processor */

	if ((inpf = fopen(name, "r")) == NULL) {
		if (!quiet)
			error(0, "Can't read %s", name);
		return;
	}
	while ((bp = fgets(buf, sizeof buf, inpf)) != NULL)
		if (*bp != '\n')
			break;
	if (bp == NULL) {
		error(0, "%s: wrong format", name);
		(void) fclose(inpf);
		return;
	}
	for (fp = fsw; fp->type; fp++)
		if (!strncmp(fp->type, buf, strlen(fp->type)))
			(*fp->func) (inpf);
	(void) fclose(inpf);
}

int
main(int argc, char *argv[])
{

	int     opt;
	extern int optind;
	extern char *optarg;
	int     i;

	invo_name = argv[0];
	setbuf(stdout, _obuf);

	while ((opt = getopt(argc, argv, "c:a:p:")) != EOF) {
		switch (opt) {
		case 'c':
		case 'a':
			if ((outf = fopen(optarg,
					  opt == 'c' ? "w" : "a")) == NULL)
				error(1, "Can't open %s", optarg);
			(void) strcpy(oname, optarg);
			break;
		case 'p':
			profile_file = optarg;
			break;
		}
	}
	argv += optind;
	argc -= optind;
	profile();

	/*
	 * Process empire report files.  This is done by
	 * looking at the second line of the file which
	 * contains a string of the form:
	 *
	 * cen ... (e.g. cen # >cen.out)
	 *
	 * The first word indicates the type of file it is
	 * (census, map, etc).
	 */

	if (autoload) {
		for (i = 0; preload[i]; i++)
			load_file(preload[i], TRUE);
	}
	while (argc-- > 0) {
		need_dump = TRUE;
		load_file(*argv, FALSE);
		argv++;
	}
	center(0, 0, FALSE);
	startx = 0 - MCOLS / 2;		    /* Set at Capitol */
	if (ODD(startx))
		startx--;
	starty = 0 - MLINES / 2;
	if (ODD(starty))
		starty--;

	(void) signal(SIGINT, SIG_IGN);	    /* Ignore common signals */
	(void) signal(SIGQUIT, SIG_IGN);

	setlocale(LC_ALL, "");
	xtn_init();
	// ESCDELAY = 20;
	(void) initscr();		    /* Start up curses etc. */
	start_color();
	use_default_colors();
	init_pair(NCOLOR_ENEMY, -1, COLOR_RED);
	init_pair(NCOLOR_FRIEND, -1, COLOR_BLUE);
	init_pair(NCOLOR_BOTH, -1, COLOR_YELLOW);

	init_pair(GCOLOR_RANGE, COLOR_RED, -1);

	init_pair(LCOLOR_ENEMY, -1, COLOR_RED);
	init_pair(LCOLOR_FRIEND, -1, COLOR_CYAN);
	init_pair(LCOLOR_BOTH, -1, COLOR_MAGENTA);

	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	incurses = TRUE;
	presetty();
	commands();			    /* Process all commands */
	move(LINES - 1, 0);		    /* Done - move to bottom
					     * of screen */
	clrtoeol();
	refresh();

	endtty();			    /* Reset terminal
					     * characteristics */
}


/*
 * merge - Update sector designation - works in transformed coords.
 * flg indicates wether to make subtle changes to '^' & '-' if got
 * from census reports.
 */
char
merge(uc, x, y, flg)
	register char uc;
	register int x, y;
	int     flg;
{
	Sector *mp;

	mp = map[x][y];
	if (mp == NULL)
		mp = map[x][y] = newmap();
	if (flg)
		switch (uc) {
		case '^':
			uc = '&';
			break;
		case '-':
			uc = '~';
			break;
		}
	switch (mp->des) {
	case '^':
		if (uc == '&')
			mp->des = uc;
		/* fall through */
	case '\\':
		break;
	case '~':
		if (uc == '-')
			break;
		/* fall through */
	case '&':
		if (uc == '^')
			break;
		/* fall through */
	default:
	case '-':
	case '?':
		if (uc == '?')
			mp->own = 2;
		if (uc == ' ' || uc == '$' || uc == '?')
			break;
		/* fall through */
	case 0:
	case ' ':
		if (uc == '$')
			mp->des = '.';
		else
			mp->des = uc;
		break;
	}
	return mp->des;
}
/*
 * center - Center display window about (x,y).
 * flag forces centering otherwise only centre if we want to.
 */
void
center(x, y, flag)
	register int x, y;
	int     flag;
{
	if (flag || x - 10 < startx || x + 10 > startx + MCOLS ||
	    y - 4 < starty || y + 4 > starty + MLINES) {
		startx = x - MCOLS / 2;
		starty = y - MLINES / 2;
	}
}


/*
 * clearmks - Clear sector marks.  If 'all' flag is set, then ALL marks
 *            cleared, otherwise just curmark marks are cleared.
 */
void
clearmks(all)
	int     all;
{
	register int x, y;
	Sector *mp;

	for (y = min_y; y <= max_y; y++)
		for (x = min_x; x <= max_x; x++)
			if ((mp = map[x][y]) == NULL)
				continue;
			else if (all || mp->mark == curmark)
				mp->mark = 0;
}

/*
 * display a selection list in a popup
 * return the selected item
 */

char *popsel(int rows, int cols, const Slist *data)
{
	WINDOW *popup, *pbord;
	bool showing = true;
	int xoff=0, yoff=0, y;
	int crow=0, trow=0;

	pbord = newwin(rows+2, cols+2, 1, 1);
	popup = derwin(pbord, rows, cols, 1, 1);
	box(pbord, 0,0);
	wrefresh(pbord);
	while (showing) {
		for (y=0; y<data->n; y++) {
			char *t = slist_item(data, y)->val;
			int l = cols - strlen(t);
			wstandout(popup);
			mvwaddstr(popup, y, 0, t);
			for(;l>0; --l) waddch(popup, ' ');
			wstandend(popup);
		}
		wrefresh(popup);
		switch (getch()) {
			case KEY_UP: yoff--; break;
			case KEY_DOWN: yoff++; break;
			case 'q':
			case '\x1b':
			default:
			    showing=false; break;
		}
	}
	delwin(popup);
	delwin(pbord);
	return 0;
}

void popup(int rows, int cols, int drows, const char **data)
{
	WINDOW *popup, *pbord;
	bool showing = true;
	int xoff=0, yoff=0, y;

	pbord = newwin(rows+2, cols+2, 1, 1);
	popup = derwin(pbord, rows, cols, 1, 1);
	box(pbord, 0,0);
	wrefresh(pbord);
	while (showing) {
		// wclear(popup);
		for (y=0; y<rows && y<drows; y++) {
			mvwaddstr(popup, y, 0, *(data+y+yoff));
		}
		wrefresh(popup);
		switch (getch()) {
			case KEY_UP: yoff--; break;
			case KEY_DOWN: yoff++; break;
			case 'q':
			case '\x1b':
			default:
			    showing=false; break;
		}
		if (yoff>(drows-rows)) yoff=drows-rows;
		if (yoff<0) yoff=0;
	}
	delwin(popup);
	delwin(pbord);
}

void showhelp()
{
	popup(20, 60, help_cnt, help);
}

void
ori_showhelp()
{
	WINDOW *helpwin;
	int i = 0;

	helpwin = newwin(20, 60, 2, 2);
	for (i=0; i < help_cnt; ++i) {
		waddstr(helpwin, help[i]);
	}
	box(helpwin, 0,0);
	wgetch(helpwin);
	delwin(helpwin);
}

/*
 * commands - Process input commands.
 */
void
commands()
{
	register int c;
	int     x, y;			    /* Indexes into map
					     * array */
	register Item *ip;		    /* Pointer to current
					     * item */
	int     tx, ty;			    /* Temporary x,y */
	int     crou = -1;		    /* Current route */
	int     status;			    /* Fork status return */
	int     pflg = 1;		    /* Print census flag */
	int     update = 0;		    /* Set if screen update
					     * needed */
	Sector *mp;
	char   *bp;
	char    prbuf[BUFSIZ];
	char    buf[BUFSIZ];
	char   *getenv();

	center(x = 0, y = 0, TRUE);

	mapdr(NOSU);			    /* Draw map, census
					     * header and census */
	censusinfo(x, y);

	move(y - starty, x - startx);

	while (refresh(), ((c = getac()) != 'q' && c != EOF)) {
		update = 0;		    /* Initialize variables
					     * for next command */
		curx = x;
		cury = y;
		if (!xtn_execbind(c, &x, &y))
		switch (c) {
		case 'h':
			showhelp();
			mapdr(surmap);
			break;
			/* Movement commands */
		case 'y':
			x--;
			y--;
			break;

		case 'u':
			x++;
			y--;
			break;

		case KEY_UP:
		if (x & 0x1 == 1) {
				x++;
			} else {
				x--;
			}
			y--;
			break;

		case KEY_RIGHT:
		case 'j':
			x += 2;
			break;

		case 'n':
			x++;
			y++;
			break;

		case 'b':
			x--;
			y++;
			break;

		case KEY_DOWN:
			if (x & 0x1 == 1) {
				x++;
			} else {
				x--;
			}
			y++;
			break;

		case KEY_LEFT:
		case 'g':
			x -= 2;
			break;

		/* scroll */
		case 0x002:
		case 0x00E:
			y += 6;
			break;

		case 0x015:
		case 0x019:
			y -= 6;
			break;

		case 0x007:
			x -= 6;
			break;

		case 0x00A:
			x += 6;
			break;

		case 0x012:
			center(curx, cury, TRUE);
		case 0x00C:
			clear();	    /* Redraw the screen */
			refresh();
			mapdr(surmap);
			censusinfo(x, y);
			break;

		case '?':
			query();	    /* Do query at bottom */
			mapdr(surmap);
			censusinfo(x, y);
			break;

		case '!':		    /* Fork a shell */
			if ((bp = getenv("SHELL")) == NULL)
				(void) strcpy(prbuf, SHPATH);
			else
				(void) strcpy(prbuf, bp);
			if (outf)
				(void) fclose(outf);
			if (fork() == 0) {
				move(LINES - 1, 0);
				clrtoeol();
				refresh();
				endtty();
				execl(prbuf, prbuf, 0);
				_exit(1);
			} else
				(void) wait(&status);
			presetty();
			clear();
			mapdr(surmap);
			censusinfo(x, y);
			if (outf)
				if ((outf = fopen(oname, "a")) == NULL)
					putline("Cannot reopen %s", oname);
			break;
		case '+':
		case '-':
		case '/':
			checkshiporplane(c, x, y);
			/* Advance to next ship */
			update++;
			break;

		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8':
		case '9':
			drawrange(x, y, c-'0');
			mapdr(surmap);
			break;

		case KEY_BACKSPACE:
			clearovl();
			mapdr(surmap);
			break;

		case 'C':
		case 'c':
			clearmks(c == 'C'); /* Clear marks */
			mapdr(surmap);
			break;

		case 'M':
			curmark = '>';	    /* Reset mark */
			break;

		case 'm':
			ve_getline(prbuf, "mark: ", NOX);
			/* Change mark */
			if (*prbuf)
				curmark = *prbuf;
			break;

		case 'G': /* Leap to ship */
			locateship(&x, &y, surmap);
			update++;
			break;

		case 'B': /* Leap to unit */
			locateunit(&x, &y, surmap);
			update++;
			break;

		case 'T': /* Leap to plane */
			locateplane(&x, &y, surmap);
			update++;
			break;

		case 'P':
			pflg = (pflg == 0);
			/* Toggle census refresh */
			move(LINES - 1, 0);
			clrtoeol();
			printw("Printing %s", pflg ? "on" : "off");
			break;

		case 'U':		    /* Toggle unit display */
			unitmode = (unitmode == FALSE);
			mapdr(surmap);
			censusinfo(x, y);
			break;

		case 'N':		    /* Toggle ship display */
			shipmode = (shipmode == FALSE);
			mapdr(surmap);
			censusinfo(x, y);
			break;

		case 'A':		    /* Toggle plane display */
			planemode = (planemode == FALSE);
			mapdr(surmap);
			censusinfo(x, y);
			break;

		case 'a':
			if (outf) {	    /* Append to the file */
				ve_getline(prbuf, "", EX);
				x = curx;
				y = cury;
				if (*prbuf) {
					(void) strcat(prbuf, "\n");
					fputs(prbuf, outf);
					(void) fflush(outf);
				}
			} else
				putline("No output file specified - use O/o");
			break;

		case 'i':		    /* Read in input data
					     * (map,...) */
		case 'I':
			ve_getline(buf, "New input file: ", NOX);
			need_dump = TRUE;
			load_file(buf, FALSE);
			mapdr(surmap);
			censusinfo(x, y);
			break;

		case 'o':		    /* Change/create output
					     * file */
		case 'O':
			ve_getline(buf, "New output file: ", NOX);
			if (outf)
				(void) fclose(outf);
			if ((outf = fopen(buf, c == 'O' ? "w" : "a")) == NULL)
				putline("%s: cannot create", buf);
			else
				(void) strcpy(oname, buf);
			break;

		case 's':		    /* Set a macro */
			ve_getline(buf, "macro name: ", NOX);
			ve_getline(prbuf, "define: ", NOX);
			macros[*buf] = strdup(prbuf);
			break;

		case 'd':		    /* Delete a macro */
			ve_getline(buf, "delete macro: ", NOX);
			if (macros[*buf])
				free(macros[*buf]);
			macros[*buf] = 0;
			break;

		case 'D':		    /* a gal special */
			dump();
			need_dump = FALSE;
			break;

		case 'L':
			levelmode = (levelmode + 1) % 3;
			mapdr(surmap);
			censusinfo(x, y);
			break;

		case 'V':
			if ((bp = getenv("VISUAL")) == NULL)
				(void) strcpy(prbuf, VIPATH);
			else
				(void) strcpy(prbuf, bp);
			goto forkeditor;

		case 'E':
			if ((bp = getenv("EDITOR")) == NULL)
				(void) strcpy(prbuf, EXPATH);
			else
				(void) strcpy(prbuf, bp);
	forkeditor:	if (outf) {
				(void) fclose(outf);
				move(LINES - 1, 0);
				clrtoeol();
				refresh();
				if (fork() == 0) {
					endtty();
					execl(prbuf, prbuf, oname, 0);
					_exit(1);
				} else
					(void) wait(&status);
				presetty();
				clear();
				mapdr(surmap);
				censusinfo(x, y);
				if ((outf = fopen(oname, "a")) == NULL)
					putline("Cannot reopen %s", oname);
			} else
				putline("No output file");
			break;

		case 'S':		    /* Survey */
			ve_getline(buf, "Survey: ", NOX);
			survey(buf);
			mapdr(surmap = SURV);
			break;

		case 'R':		    /* Range for Survey */
			sprintf(prbuf, "Range (%d): ", range * 10);
			ve_getline(buf, prbuf, NOX);
			if (*buf) {
				range = atoi(buf) / 10;
				if (range < 1) {
					putline("range should be >= 10");
					range = 10;
				}
			}
			break;

		case 0x006:		    /* Flip maps */
			mapdr((surmap = (surmap == NOSU)));
			break;

		case 'r':		    /* Trace route */
			ve_getline(buf, "Route: ", FALSE);
			for (crou = -1, ip = items; ip->len; ip++)
				if (!strncmp(buf, ip->nm, ip->len)) {
					crou = ip->value;
					break;
				}
			if (crou < 0)
				putline("I don't know about %s", buf);
			break;

		case 'w':		    /* Walk along route */
			if ((mp = map[xoffset(x)][yoffset(y)]) == NULL)
				break;
			if (crou > -1 && mp->vp &&
			    mp->vp->del[crou] != '.')
				peekc = mp->vp->del[crou];
			break;

		case 'p':
			break;		    /* Print census */

		case 'v':		    /* print version */
			version();
			break;

		case 'l':
			ve_getline(buf, "Leap to: ", TRUE);
			if (*buf && (bp = (char *) index(buf, ','))) {
				tx = atoi(buf);
				ty = atoi(++bp);
				if (!VALID(tx, ty)) {
					tx -= ODD(tx);
					ty -= ODD(ty);
				}
				x = tx;
				y = ty;
				center(x, y, FALSE);
				update++;
			}
			break;
		case ':':
			ve_getline(buf, "> ", FALSE);
			xtn_setxy(x, y);
			if (xtn_eval(buf)) {
				mapdr(surmap);
			}
			xtn_getxy(&x, &y);
			break;
		case 'H': {
			Slist *s = getships();
			popsel(20, 60, s);
			mapdr(surmap);
			slist_free(s);
			break;
		}
		default:
			beep();
		}
		x %= MAPSIZE;
		y %= MAPSIZE;
		if (x < startx || x > startx + MCOLS) {
			startx = (x < startx) ? x : x - MCOLS;
			update++;
		}
		if (y < starty || y > starty + MLINES) {
			starty = (y < starty) ? y : y - MLINES;
			update++;
		}
		if (update) {
			mapdr(surmap);
			censusinfo(x, y);
			touchwin(stdscr);
		} else {
			if (pflg || c == 'p')
				censusinfo(x, y);
		}
		move(y - starty, x - startx);
	}
	if (need_dump && autowrite)
		dump();
}


/*
 * endtty - Restore terminal to normal mode.
 */
void
endtty()
{
	endwin();
	incurses = FALSE;
}


/*
 * getac - Get a character. Return peekc if non-zero,
 *         otherwise read a character from the keyboard.
 */
int
getac()
{
	register char tc;

	if (peekc) {
		tc = peekc;
		peekc = 0;
		return (tc);
	} else
		return (getch());
}


/*
 * ve_getline - Get input line from the bottom of the screen,
 *           using pr as a prompt if non-zero. If ex is set,
 *           then expand macros.
 */
void
ve_getline(bp, pr, ex)
	char   *bp;
	char   *pr;
	int     ex;
{
	register int x, y;
	register int c;
	char   *mp;
	char   *np;
	char    nbuf[10];		    /* Number buffer */
	char   *ip = bp;

	move(LINES - 1, 0);
	clrtoeol();
	if (*pr)
		addstr(pr);
	while (refresh(), (c = getch()) != '\r' && c != '\n' && c != EOF) {
		if (ex && macros[c]) {	    /* check for macros */
			mp = macros[c];
			while (*mp) {
				if (*mp == '.') {	/* expand . */
					mp++;
					sprintf(np = nbuf, "%d,%d ", curx, cury);
					while (*ip++ = *np++);
					ip--;
					addstr(nbuf);
				} else
					addch(*ip++ = *mp++);
			}
			continue;
		}
		switch (c) {
		case KEY_BACKSPACE:
		case KEY_DC:
		case '\b':
			if (ip > bp) {
				ip--;
				*ip = 0;
				addstr("\b \b");
			}
			continue;

		case '\\':
			addstr("\\\b");	    /* backslash */
			refresh();
			c = getch();
			break;

		case '.':
			if (ex) {	    /* expand . */
				(void) sprintf(np = nbuf, "%d,%d ", curx, cury);
				while (*ip++ = *np++);
				ip--;
				*ip = 0;
				addstr(nbuf);
				continue;
			} else
				break;

		case ESC:
			getyx(stdscr, y, x);
			/* jump to current x,y */
			move(cury - starty, curx - startx);
			refresh();
			sleep(1);
			move(y, x);
			continue;

		case '\n':
			continue;

		case '\f':
			wrefresh(curscr);
			continue;

		case CTRL('p'):
			if (ex)
				c = processmove(bp, &ip);
			if (c > ' ')
				break;
			continue;

		case KEY_EOL:
		case CTRL('u'):
			move(LINES - 1, 0); /* erase the line */
			if (*pr)
				addstr(pr);
			clrtoeol();
			*(ip = bp) = 0;
			continue;
		}
		addch(*ip++ = c);
		*ip = 0;
	}
	*ip = 0;
}


/*
 * getnewxy - Get last postion of MOVE or NAV command.
 */
int
getnewxy(bp, x, y)
	char   *bp;
	int    *x, *y;
{
	register char *ip = bp;
	char   *findblank();
	char   *findchar();
	char    fleet;
	char    xbuf[6];
	char    ybuf[6];
	int     oldy, oldx;
	int     shipnumber;
	Sector *mp;
	register int i;

	switch (*ip) {
	case 'm':
	case 'e':
		if ((strncmp(ip, "mov", 3) != 0) &&
		    (strncmp(ip, "exp", 3) != 0))
			return (0);
		if (*(ip = findblank(ip)) == 0)
			return (0);
		if (*(ip = findchar(ip)) == 0)
			return (0);
		if (*(ip = findblank(ip)) == 0)
			return (0);
		if (*(ip = findchar(ip)) == 0)
			return (0);
		for (i = 0; (i < 5) && (*ip != ',') && (*ip != 0);
		     i++) {
			xbuf[i] = *ip++;
			xbuf[i + 1] = 0;
		}
		if ((*ip != ',') || (*ip == 0))
			return (0);
		ip++;
		for (i = 0; (i < 5) && (*ip != ' ') && (*ip != 0);
		     i++) {
			ybuf[i] = *ip++;
			ybuf[i + 1] = 0;
		}
		if ((*ip != ' ') || (*ip == 0))
			return (0);
		*x = atoi(xbuf);
		*y = atoi(ybuf);
		if (*(ip = findchar(ip)) == 0)
			return (0);
		if (*(ip = findblank(ip)) == 0)
			return (0);
		if (*(ip = findchar(ip)) == 0)
			return (1);
		break;

	case 'n':
		if (strncmp(ip, "nav", 3) != 0)
			return (0);
		if (*(ip = findblank(ip)) == 0)
			return (0);
		if (*(ip = findchar(ip)) == 0)
			return (0);
		shipnumber = UNKNOWN;
		fleet = *ip;
		if ((fleet >= '0') && (fleet <= '9')) {
			for (i = 0; (i < 5) && (*ip != '/') &&
			     (*ip != ' ') && (*ip != 0);
			     i++) {
				xbuf[i] = *ip++;
				xbuf[i + 1] = 0;
			}
			shipnumber = atoi(xbuf);
		}
		if ((i = findship(shipnumber, fleet)) == NOSHIPS)
			return (0);
		*x = ships[i]->x;
		*y = ships[i]->y;
		if (*ip != ' ')
			ip = findblank(ip);
		if (*ip == 0)
			return (0);
		shipmode = TRUE;
		if ((mp = map[xoffset(*x)][yoffset(*y)]) == NULL)
			map[xoffset(*x)][yoffset(*y)] = mp = newmap();
		mp->shp = i;
		getyx(stdscr, oldy, oldx);
		curx = *x;
		cury = *y;
		center(curx, cury, FALSE);
		mapdr(surmap);
		touchwin(stdscr);
		censusinfo(curx, cury);
		move(oldy, oldx);
		if (*(ip = findchar(ip)) == 0)
			return (1);
		break;

	default:
		return (0);
	}

	while (*ip != 0) {
		switch (*ip) {
		case 'y':
			(*x)--;
			(*y)--;
			break;

		case 'u':
			(*x)++;
			(*y)--;
			break;

		case 'j':
			*x += 2;
			break;

		case 'n':
			(*x)++;
			(*y)++;
			break;

		case 'b':
			(*x)--;
			(*y)++;
			break;

		case 'g':
			*x -= 2;
			break;

		default:
			return (0);
		}
		ip++;
	}
	return (1);
}
/*
 * mark - Mark map according to command.
 */
void
mark(sp, pass)
	char   *sp;
	int     pass;
{
	register int itm1;
	register int itm2;
	register int x, y;
	register int num;
	register int val;
	register int markit;
	register Item *ip;
	register Item *tp;
	Value  *vp;
	char    cmd[20];
	char   *cp = cmd;
	Sector *mp;

	while (isalpha(*cp++ = *sp++));	    /* get first word */
	*--cp = 0;
	--sp;
	for (ip = items; ip->len; ip++)	    /* check it */
		if (!strncmp(cmd, ip->nm, ip->len))
			break;

	if (!ip->len) {
		putline("I don't know about %s\n", cmd);
		return;
	}
	itm1 = ip->value;
	/*
	 * At this point, cmd contains the left side, *sp
	 * is the operator and sp+1 is the right side.
	 */
	switch (itm1) {
	case DES:
	case SDES:
	case CHKPT:
		num = *(sp + 1);
		break;
	case DIST:
	case DEL:
		for (tp = items; tp->len; tp++)
			if (!strncmp(sp + 1, tp->nm, tp->len))
				break;
		if (!tp->len) {
			putline("I don't know about %s", cmd);
			return;
		}
		itm2 = tp->value;
		break;
	default:
		if (*sp) {
			num = atoi(sp + 1);
		}
		break;
	}
	for (y = min_y; y <= max_y; y++)
		for (x = min_x; x <= max_x; x++) {
			mp = map[x][y];
			if (mp == NULL)
				continue;
			if (!VALID(x, y) || !mp->own || mp->des == '.'
			    || mp->vp == NULL)
				continue;
			vp = mp->vp;
			if (vp->val[COU] != -1)
				continue;
			markit = 0;
			val = vp->val[itm1];
			if (itm1 == DIST) {
				num = '$';
				val = vp->dist[itm2];
			} else {
				if (itm1 == DEL) {
					num = '.';	/* KLUDGE */
					*sp = '#';
					val = vp->del[itm2];
				}
			}
			switch (*sp) {
			case '=':
				if (val == num)
					markit++;
				break;

			case '#':
				if (val != num)
					markit++;
				break;

			case '>':
				if (val > num)
					markit++;
				break;

			case '<':
				if (val < num)
					markit++;
				break;
			case '\0':
				if (val)
					markit++;
				break;
			}
			if (markit && (!pass || (pass && mp->mark)))
				mp->mark = curmark;
			else if (mp->mark == curmark)
				mp->mark = 0;
		}
}


/*
 * presetty - Set up terminal for display mode.
 */
void
presetty()
{
	cbreak();
	noecho();
/*        nonl();	*/
}
/*
 * processmove - process MOVE or NAV command.
 */
char
processmove(bp, ip)
	char   *bp;
	char  **ip;
{
	register int c;
	int     x, y;
	int     oldx, oldy;

	if (getnewxy(bp, &x, &y) == 0)
		return (0);
	getyx(stdscr, oldy, oldx);
	updatescreen(x, y);
	while (refresh(), (c = getch()) != EOF) {
		switch (c) {
		case 'y':
			x--;
			y--;
			break;

		case 'u':
			x++;
			y--;
			break;

		case KEY_UP:
			if (x & 0x1 == 1) {
				x++;
				c = 'u';
			} else {
				x--;
				c = 'y';
			}
			y--;
			break;

		case KEY_RIGHT:
			c = 'j';

		case 'j':
			x += 2;
			break;
			
		case 'n':
			x++;
			y++;
			break;

		case 'b':
			x--;
			y++;
			break;

		case KEY_DOWN:
			if (x & 0x1 == 1) {
				x++;
				c = 'n';
			} else {
				x--;
				c = 'b';
			}
			y++;
			break;

		case KEY_LEFT:
			c = 'g';

		case 'g':
			x -= 2;
			break;

		case KEY_BACKSPACE:
		case KEY_DC:
		case '\b':
			(*ip)--;
			**ip = 0;
			move(oldy, oldx);
			addstr("\b \b");
			oldx--;
			getnewxy(bp, &x, &y);
			updatescreen(x, y);
			continue;

		default:
			updatescreen(curx, cury);
			move(oldy, oldx);
			refresh();
			return (c);
		}
		move(oldy, oldx);
		**ip = c;
		(*ip)++;
		**ip = 0;
		addch(c);
		oldx++;
		updatescreen(x, y);
	}
	/* NOTREACHED */
}


/*
 * putline - Do a printw at the bottom of the screen.
 */
void
putline(char *fmt, ...)
{
	va_list	ap;
	char	buf[ 1024 ];

	va_start(ap, fmt);
	(void)uprintf(buf, fmt, &ap);
	va_end(ap);
	if (!incurses) {
		fputs(buf, stdout);
		return;
	}
	printAtBot(buf);
}

/*
 * query - Parse ? command.
 */
void
query()
{
	register int pass = 0;
	char    buf[BUFSIZ];
	char   *bp = buf;
	char   *tp;

	ve_getline(bp, "?", NOX);
	if (!*bp)
		return;
	clearmks(0);
	for (;;) {
		if (tp = (char *) index(bp, '&')) {
			*tp++ = 0;
			mark(bp, pass);
			bp = tp;
		} else {
			mark(bp, pass);
			break;
		}
		pass++;
	}
}
/*
 * survey - Display survey info for a given item.
 */
void
survey(sp)
	char   *sp;
{
	register Item *ip;
	register int itm;
	register int x, y;
	register Sector *mp;
	register int surval;

	for (ip = items; ip->len; ip++)
		if (!strncmp(sp, ip->nm, ip->len))
			break;

	if (!ip->len) {
		putline("I don't know about %s", sp);
		return;
	}
	itm = ip->value;
	for (y = min_y; y <= max_y; y++)
		for (x = min_x; x <= max_x; x++) {
			mp = map[x][y];
			if (mp == NULL)
				continue;
			if (mp->own && mp->des != '.' && mp->vp) {
				surval = mp->vp->val[itm] / range;
				if (surval > 35)
					mp->surv = '$';
				else {
					mp->surv = surval;
					mp->surv += (mp->surv > 9) ? ('A' - 10) : '0';
				}
			}
		}
}
/*
 * updatescreen - Update screen if display window moved.
 */
void
updatescreen(x, y)
	register int x, y;
{
	register int update = 0;

	if (x < startx || x > startx + MCOLS) {
		startx = (x < startx) ? x : x - MCOLS;
		update++;
	}				    /* if */
	if (y < starty || y > starty + MLINES) {
		starty = (y < starty) ? y : y - MLINES;
		update++;
	}				    /* if */
	if (update) {
		mapdr(surmap);
		touchwin(stdscr);
	}				    /* if */
	censusinfo(x, y);
	move(y - starty, x - startx);
	refresh();
}
/*
 * xoffset - Return transformed xcoordinate. Eventually, should be able
 * to work with different x & y map sizes.
 */
int
xoffset(c)
	register int c;
{
	c %= XMAPSIZE;			    /* c in range +/-
					     * MAPSIZE */
	if (c < 0)
		c += XMAPSIZE;		    /* c now in range 0 -
					     * MAPSIZE */
	c = (c + XMAPSIZE / 2) % XMAPSIZE;    /* c now in range 0 -
					     * MAPSIZE with 0,0 at
					     * MAPSIZE/2 */
	if (c < 0 || c >= XMAPSIZE)	    /* safety - should
					     * "never happen" */
		error(0, "Coordinate out of range %d", c);
	return c;
}
/*
 * yoffset - Return transformed ycoordinate.
 */
int
yoffset(c)
	register int c;
{
	c %= YMAPSIZE;
	if (c < 0)
		c += YMAPSIZE;
	c = (c + YMAPSIZE / 2) % YMAPSIZE;
	if (c < 0 || c >= YMAPSIZE)
		error(0, "Coordinate out of range %d", c);
	return c;
}
/*
 * newval - allocate a new Value structure.
 */
Value  *
newval()
{
	register Value *vp;

	vp = (Value *) calloc(1, sizeof(*vp));
	if (vp == NULL)
		error(1, "Out of memory in newval");
	return vp;
}
/*
 * newmap - allocate a new Sector struct
 */
Sector *
newmap()
{
	register Sector *sp;

	sp = (Sector *) calloc(1, sizeof(*sp));
	if (sp == NULL)
		error(1, "Out of memory in newmap");
	sp->shp = NOSHIPS;
	sp->pln = NOPLANES;
	sp->unt = NOUNITS;
	return sp;
}
/*
 * checkshiporplane - Determines whether ships or planes are at the
 *		      present location.
 */
void
checkshiporplane(c, x, y)
	char    c;
	int     x, y;
{
	register struct sector *mp = map[xoffset(x)][yoffset(y)];
	int     mode;

	mode = (shipmode && mp && mp->shp != NOSHIPS) +
	    2 * (planemode && mp && mp->pln != NOPLANES) +
		4 * (unitmode && mp && mp->unt != NOUNITS);
	switch (mode) {
	case 0:
		break;			    /* Ships and planes off */

	case 1:			    /* Ships only */
		switch (c) {
		case '+':
			nextship(x, y);
			break;
		case '-':
			previousship(x, y);
			break;
		case '/':
			firstship(x, y);
			break;
		default:
			break;
		}

	case 2:			    /* Planes only */
		switch (c) {
		case '+':
			nextplane(x, y);
			break;
		case '-':
			previousplane(x, y);
			break;
		case '/':
			firstplane(x, y);
			break;
		default:
			break;
		}
	case 3:			    /* Both planes and ships */
		break;
	case 4:			    /* Units only */
		switch (c) {
		case '+':
			nextunit(x, y);
			break;
		case '-':
			previousunit(x, y);
			break;
		case '/':
			firstunit(x, y);
			break;
		default:
			break;
		}
	default:
		break;
	}
}
/*
 * version Print version of ve
 */
void
version()
{
	move(LINES - 1, 0);
	clrtoeol();
	printw("ve Version 5.1");
	printw(" May, 26, 1992");
	printw("    This version is setup for Empire 1.0");
}
void
maxxy(x, y)
	int     x, y;
{
	if (x < min_x)
		min_x = x;
	if (x > max_x)
		max_x = x;
	if (y < min_y)
		min_y = y;
	if (y > max_y)
		max_y = y;
}
