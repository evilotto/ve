
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

#define	VALUESIZE	45		    /* Number of values in
					     * sector */

/*
 * If you want accounting, change the filename below (make sure
 * it is 622 at	least).	 Everytime someone plays that version
 * a line will be entered showing login	name, elapsed time and
 * date.
 */

/*# define	ACCTNG	"/tmp/ve-acct"*/
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

extern Sector *map[MAPSIZE][MAPSIZE];
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
extern void putline(char *fmt, ...);
extern void error(int type, char *fmt, ...);
extern char *uprintf(char *buf, char *fmt, ...);
extern void printAtBot(char *buf);
extern char processmove(char *bp, char **ip);
