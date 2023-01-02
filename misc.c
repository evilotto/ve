/* misc.c - miscellaneous routines */

# include "ve.h"
#include <errno.h>

skipword(fp)
	FILE   *fp;
{
	register int c;

	for (;;) {
		c = getc(fp);
		if (c == '\n' || c == ' ' || c == '\t')
			return (1);
		if (c == EOF)
			return (0);
	}
}


pitchline(fp)
	FILE   *fp;
{
	register int c;

	while ((c = getc(fp)) != '\n')
		if (c == EOF)
			break;

}
/*
 * findchar - Find next non-blank character.
 */
char   *
findchar(addr)
	char   *addr;
{
	while ((*addr != 0) && (*addr == ' '))
		addr++;
	return (addr);
}
/*
 * findblank - Find next blank character.
 */
char   *
findblank(addr)
	register char *addr;
{
	while ((*addr != 0) && (*addr != ' '))
		addr++;
	return (addr);
}					    /* findblank */
/*
 * bfindchar - Find previous non-blank character.
 */
char   *
bfindchar(addr)
	char   *addr;
{
	while ((*addr != 0) && (*addr == ' '))
		addr--;
	return (addr);
}
/*
 * bfindblank - Find previous blank character.
 */
char   *
bfindblank(addr)
	register char *addr;
{
	while ((*addr != 0) && (*addr != ' '))
		addr--;
	return (addr);
}					    /* findblank */

void printAtBot(str)
     char *str;
{
	move(LINES - 1, 0);
	addstr(str);
	clrtoeol();
}

extern char *invo_name;
void
error(int type, char *fmt, ...)
{
	va_list ap;
	char buf[ 1024 ];

	va_start(ap, fmt);
	(void)uprintf(buf, fmt, &ap);
	va_end(ap);
	if (incurses) {
		printAtBot(buf);
		if (type)
			exit(type);
		return;
	}
	fprintf(stderr, "%s: %s", invo_name, buf);
	fprintf(stderr, "[%s]\n", strerror(errno));
	if (type)
		exit(type);
}
