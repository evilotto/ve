
/* ve.h - ve definitions & global references */

#include <ctype.h>
#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/ttydefaults.h>

/* Things you may want to change */
#define	VIPATH		"/usr/bin/vi"
#define	SHPATH		"/bin/sh"
#define EXPATH          "/usr/bin/ex"
#define PROFILE		".ve-prof"
#define VEOPTION	"VEPROFILE"
#define SAVMAP		".map"
#define SAVCOM		".com"
#define SAVCEN		".cen"
#define SAVDUMP		".dump"

#define MAX_PRE		3

#define MAXNOC      80			    /* Maximum number of
					     * countries */
#define	MAPSIZE	    1024		    /* Empire world size */
#define	XMAPSIZE	140		    /* Empire X world size */
#define	YMAPSIZE	68		    /* Empire Y world size */
#define	RADARSIZE   80			    /* Radar scan size */
#define	MAXUNITS    1024		    /* Maximum number of
					     * units */
#define	MAXSHIPS    1024		    /* Maximum number of
					     * ships */
#define MAXPLANES   1024		    /* Maximum number of
					     * planes */
#define	DELSIZE	    35			    /* Number of deliveries
					     * per sector */
#define	DISTSIZE    37			    /* Number of
					     * distribution
					     * thresholds / sector */
#define	NODISK	    -1			    /* No disk address flag	 */
#define	NOUNITS	    -1			    /* No units present */
#define	NOSHIPS	    -1			    /* No ships present */
#define NOPLANES    -1			    /* No plane present */
#define	UNKNOWN	    -1			    /* Unknown ship index
					     * flag */
#define	YOURS	    -2			    /* Your own ship index
					     * flag */
#define	MCOLS	    (COLS-1)		    /* Width of the	map
					     * window */
#define	MLINES	    (LINES-7)		    /* Height of the map
					     * window */
#define	NOX	    0			    /* Expansion flags for
					     * getline */
#define	EX	    1			    /* */
#define	NOSU	    0			    /* Survey or no
					     * survey for mapdr */
#define	SURV	    1
#define	ESC	    '\033'

#define	ODD(x)	    ((x)&01)
#define	EVEN(x)	    (!((x)&01))
#define	max(A,B)    ((A) > (B) ? (A) : (B))
#define	min(A,B)    ((A) < (B) ? (A) : (B))
#define	VALID(x,y)  (EVEN(x+y))
#define distval(n) ((n) == 0 ? '.' : (n) < 1000 ? ((n)/100 + '0'): ((n)/1000 + 'a' - 1))

#ifndef CTRL
#define	CTRL(c)	    (c-0100)
#endif

/* Offsets into	sector arrays for various items	*/
#define	CIV	0
#define	MIL	1
#define UW	2
#define	FOOD	3
#define	SH	4
#define	GUN	5
#define	PET	6
#define	IRON	7
#define	DUST	8
#define	BAR	9
#define	CRU	10
#define	LCM	11
#define	HCM	12
#define RAD	13
#define	EFF	14
#define	MOB	15
#define	MIN	16
#define	GOLD	17
#define	FERT	18
#define	OIL	19
#define URAN	20
#define WORK	21
#define	DES	22
#define SDES	23
#define	COU	24
#define DEL	25
#define DIST	26
#define	CHKPT	27
#define PL	28
#define TECH	29
#define ATT	30
#define DEF	31
#define RANGE	32
#define SHIP	33
#define NUKE	34
#define SPECIAL	35
#define TERR	36
#define	AVAIL	37
#define ROADS	38
#define FORT	39
#define FUEL	40
#define RETR	41
#define RADIUS	42
#define XL	43
#define HE	44
#define LN	45

/* IFF values */
#define IFF_NONE 0
#define IFF_FRIEND 0x1
#define IFF_ENEMY 0x2
#define IFF_BOTH (IFF_FRIEND | IFF_ENEMY)

/* Colors */
#define NCOLOR_ENEMY 1
#define NCOLOR_FRIEND 2
#define NCOLOR_BOTH 3

#define LCOLOR_ENEMY 4
#define LCOLOR_FRIEND 5
#define LCOLOR_BOTH 6

#define ACOLOR_ANY 7

#define GCOLOR_RANGE 8

#define	VALUESIZE	46		    /* Number of values in
					     * sector */

typedef struct value {
	short   val[VALUESIZE];		    /* Values as described
					     * by items */
	short	off;			    /* is the sector updated? */
	char    del[DELSIZE];		    /* Delivery	routes */
	char    dist[DISTSIZE];		    /* Distributions */
}       Value;

typedef struct level {
	short   dx, dy;
	char    path[11];
	short   thresh[DISTSIZE];
	short   cutoff[DELSIZE];
}       Level;

typedef struct overlay {
	short color;
}		Overlay;
/*
 * Map sector data structures
 */
typedef struct sector {
	char    surv;			    /* Survey value */
	char    mark;			    /* Mark character */
	bool    own;			    /* Do you own or know
					     * that sector? */
	char    des;			    /* Sector designation */
	char    sdes;			    /* Secondary sector
					     * designation */
	short   unt;			    /* Index of current unit
					     * at x,y */
	short   shp;			    /* Index of current ship
					     * at x,y */
	short   pln;			    /* Index of current
					     * plane at x,y */
	Value  *vp;			    /* the value structure */
	Level  *lp;
}       Sector;

typedef struct unitentry {
	char    type[16];		    /* Unit type */
	char    des;			    /* Unit designation */
	char    army;			    /* Army designation */
	char    carry[5];			/* carrying ship/plane */
	short   number;			    /* Unit number */
	short   x, y;			    /* Map coordinates of
					     * unit */
	Value  *vp;			    /* the value structure */
}       Unit;

typedef struct shipentry {
	char    type[16];		    /* Ship type */
	char    des;			    /* Ship designation */
	char    fleet;			    /* Fleet designation */
	short   number;			    /* Ship number */
	short   x, y;			    /* Map coordinates of
					     * ship */
	Value  *vp;			    /* the value structure */
}       Ship;

typedef struct planeentry {
	char    type[16];		    /* Plane type */
	char    des;			    /* Plane designation
					     * (type) */
	char    wing;			    /* Wing designation */
	short   number;			    /* Plane number */
	short   x, y;			    /* Map coordinates of
					     * plane */
	Value  *vp;			    /* Disk address
					     * containing values */
}       Plane;


typedef struct item {
	char   *nm;			    /* Item name - prefix */
	short   len;			    /* Length of prefix (for
					     * strncmp) */
	short   value;			    /* What it maps to */
}       Item;

typedef struct funsw {			    /* list of prefixes and
					     * functions */
	char   *type;
	int     (*func) ();
}       Funsw;

/* select list */
typedef struct Sitem {
    char *key;
	char *val;
} Sitem;
typedef struct Slist {
	int n;
	Sitem **entries;
} Slist;

Slist *mkslist();
void slist_add(Slist* s, const char *k, const char *v);
void slist_free(Slist *s);
Sitem *slist_item(const Slist *s, int n);


/* externals */
extern int chainsaw;
extern int noise;
extern int unitmode;
extern int shipmode;
extern int planemode;
extern int unitcount;
extern int shipcount;
extern int planecount;
extern int surmap;
extern int minx, miny;
extern int maxx;
extern int max_x, min_x;
extern int max_y, min_y;
extern int startx, starty;
extern int curmark;
extern int autowrite;
extern int autoload;
extern int incurses;
extern int levelmode;
extern char CountryNames[MAXNOC][16];
extern char *macros[];
extern char mapfile[], censusfile[], commodityfile[],dumpfile[];
extern char *profile_file;

extern int distance(int sx, int sy, int ex, int ey);
extern int niff(int x, int y);

/* ve.c */
extern int xoffset(int c);
extern int yoffset(int c);
extern void profile();
extern void center(int x, int y, int flag);
extern void presetty();
extern void commands();
extern void endtty();
extern void updatescreen(int x, int y);
extern int getac();
extern void query();
extern void mark();
extern void survey(char *sp);
extern void checkshiporplane(char c, int x, int y);
extern void ve_getline(char *bp, char *pr, int ex);
extern void version();
extern void maxxy(int x, int y);
extern char merge(char uc, int x, int y, int flg);
/* map.c */
extern void mapdr(int sflg);
extern void drawrange(int x, int y, int r);
extern void li(int x, int y, int r);
extern int liff(int x, int y);
/* dump.c */
extern void dump();
extern void do_map();
extern void do_dump();
extern int divert(char *name);
extern int map_rel(int x);
extern void cens();
extern void comm();
extern void ve_border(int xmin, int xmax, int inc, char *indent, char *sep);
/* ship.c */
extern int locship(int n);
extern int findship(int ship, char fleet);
extern void firstship(int x, int y);
extern void nextship(int x, int y);
extern void previousship(int x, int y);
/* plane.c */
extern int locplane(int n);
extern int findplane(int plane, char wing);
extern void firstplane(int x, int y);
extern void nextplane(int x, int y);
extern void previousplane(int x, int y);
/* unit.c */
extern int locunit(int n);
extern int findunit(int unit, char army);
extern void firstunit(int x, int y);
extern void nextunit(int x, int y);
extern void previousunit(int x, int y);
/* satellite.c */
// extern void satellite(FILE *fp);
extern int isamap(FILE *fp);
extern void getspydata(char *buf);
extern void getshipdata(char *buf);
/* census.c */
//extern void census(FILE *fp);
extern void censusinfo(int x, int y);
extern void censusheader(int mode, int lmode);
/* lookout.c */
extern void shiplookat(char *p, int cnum);
extern void shiplook(char *p, int cnum);
/* misk before cutoff */
int skipword(FILE *dp);
void pitchline(FILE *fp);
/* radar.c */
extern int readradarlines(FILE *fp, int *xp, int *yp, float *rp);
extern void presetradarmap(char rm[RADARSIZE][RADARSIZE]);
extern int readradarscan(FILE *fp, char rm[RADARSIZE][RADARSIZE], int *xr, int *yr);
extern void editradarscan(char rm[RADARSIZE][RADARSIZE], int *yr, int *xr, int  *xp, int *yp);
/* cutoff.c */
// extern void cutoff(FILE *fp);
/* level.c */
// extern void level(FILE *fp);
/* coastwatch.c */
// extern void coastwatch(FILE *fp);
/* commodity.c */
// extern void commodities(FILE *fp);

extern Sector *map[MAPSIZE][MAPSIZE];
extern Overlay *ovl[MAPSIZE][MAPSIZE];
extern Unit *units[];
extern Ship *ships[];
extern Plane *planes[];
extern Item items[];

extern Value *newval();
extern Sector *newmap();
extern Unit * newunit();
extern Ship *newship();
extern Plane *newplane();
extern Level *newlev();
extern Overlay *newovl();
extern void setovl(int x, int y, short color);
extern void clearovl();
extern void locateship(int *x, int *y, int flag);
extern void locateunit(int *x, int *y, int flag);
extern void locateplane(int *x, int *y, int flag);
extern void putline(char *fmt, ...);
extern void error(int type, char *fmt, ...);
extern char *uprintf(char *buf, char *fmt, ...);
extern void printAtBot(char *buf);
extern char processmove(char *bp, char **ip);
extern char *fmtships();
extern Slist *getships();
