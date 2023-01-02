/* misc.c - miscellaneous routines */

# include "ve.h"
#ifdef hpux
# include <sys/types.h>
#endif
#ifndef VMS
# include <fcntl.h>
#endif

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

char   *
strdup(s)
	char   *s;
{
	extern char *malloc();
	extern char *strcpy();
	auto char *p;

	if ((p = malloc((unsigned) strlen(s) + 1)) == (char *) 0)
		error(1, "Out of memmory in strdup <%d bytes needed>");
	return strcpy(p, s);
}

void printAtBot(str)
     char *str;
{
	move(LINES - 1, 0);
	addstr(str);
	clrtoeol();
}

extern char *invo_name;
/*VARARGS2*/
void
error(va_alist)
	va_dcl
{
	va_list ap;
	char *fmt, buf[ 1024 ];
	int type;

	extern int errno;
	extern int sys_nerr;
	extern char *sys_errlist[];

	va_start(ap);
	type = va_arg(ap, int);
	fmt = va_arg(ap, char *);
	(void)uprintf(buf, fmt, &ap);
	va_end(ap);
	if (incurses) {
		printAtBot(buf);
		if (type)
			exit(type);
		return;
	}
	fprintf(stderr, "%s: %s", invo_name, buf);
	if (errno > 0 && errno < sys_nerr)
		fprintf(stderr, "[%s]\n", sys_errlist[errno]);
	else
		putc('\n', stderr);
	if (type)
		exit(type);
}
