/* profile.c - profile interpretation */

# include "ve.h"

char   *profile_file;

typedef struct {
	char   *key;
	int     value;
}       Lookup;

Lookup  prof_tbl[] = {
#define	PROF_MARK	1
	"mark", PROF_MARK,
#define PROF_MACRO	2
	"macro", PROF_MACRO,
#define PROF_MAPFILE	3
	"mapfile", PROF_MAPFILE,
#define PROF_CENSUS	4
	"census", PROF_CENSUS,
#define PROF_COMMODITY	5
	"commodity", PROF_COMMODITY,
#define PROF_AUTOWRITE	6
	"autosave", PROF_AUTOWRITE,
#define PROF_COMMENT	7
	"#", PROF_COMMENT,
#define PROF_AUTOLOAD	8
	"autoload", PROF_AUTOLOAD,
#define PROF_DUMPFILE	9
	"dumpfile", PROF_DUMPFILE,
	0, 0
};
profile()
{
	char   *p;
	char    c;
	char   *getenv();
	FILE   *fp;
	char    buf[BUFSIZ];
	char   *findprofile();

	if ((p = findprofile()) == NULL)
		return;

	if ((fp = fopen(p, "r")) == NULL)
		return;
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (p = index(buf, '\n'))
			*p = '\0';

		switch (lookup(buf, &p)) {
		case PROF_COMMENT:
			continue;
		case PROF_MACRO:
			if (*p == '\0') {
				error(0, "Bad macro definition '%s'",
				      buf);
				continue;
			}
			c = *p;
			if (macros[c] != NULL)
				free(macros[c]);
			p++;
			while (isspace(*p))
				p++;
			macros[c] = strdup(p);
			break;
		case PROF_MARK:
			if (*p == '\0') {
				error(0, "Bad mark definition '%s'",
				      buf);
				continue;
			}
			curmark = *p;
			break;
		case PROF_MAPFILE:
			(void) strcpy(mapfile, p);
			break;
/*		case PROF_CENSUS:
			(void) strcpy(censusfile, p);
			break;
		case PROF_COMMODITY:
			(void) strcpy(commodityfile, p);
			break; */
		case PROF_DUMPFILE:
			(void) strcpy(dumpfile, p);
			break;
		case PROF_AUTOWRITE:
			autowrite = TRUE;
			break;
		case PROF_AUTOLOAD:
			autoload = TRUE;
			break;
		default:
			fprintf(stderr, "Unknown profile option %s\n", buf);
			break;
		}
	}
	(void) fclose(fp);
}



int 
lookup(str, ptr)
	char   *str, **ptr;
{
	Lookup *lp;
	char   *p;

	for (lp = prof_tbl; lp->key; lp++)
		if (strncmp(str, lp->key, strlen(lp->key)) == 0)
			break;
	if (lp->key == 0)
		return PROF_COMMENT;
	p = str + strlen(lp->key);
	while (*p && !isspace(*p))
		p++;
	while (isspace(*p))
		p++;
	*ptr = p;
	return lp->value;
}

char   *
findprofile()
{
	static char buf[BUFSIZ];
	register char *q, *p;

	if (profile_file)
		return profile_file;

	if ((p = getenv("HOME")) == NULL || *p == NULL)
		return NULL;

	if ((q = getenv(VEOPTION)) == NULL || *p == NULL)
		(void) sprintf(buf, "%s/%s", p, PROFILE);
	else {
		if (*p == '/')
			(void) strcpy(buf, q);
		else
			(void) sprintf(buf, "%s/%s", p, q);
	}
	return buf;
}
