EXE = ve
CFLAGS  = /define=VMS 
LINK = link
LDFLAGS = /exe=$(EXE)
LIBS	= ,sys$library:vaxccurse/lib,sys$library:vaxcrtl/lib
CC = cc
CIFLAGS	=
OBJS	= 	census.obj;,\
		coastwatch.obj;,\
		commodity.obj;,\
		cutoff.obj;,\
		dump.obj;,\
		getopt.obj;,\
		level.obj;,\
		lookout.obj;,\
		map.obj;,\
		misc.obj;,\
		plane.obj;,\
		profile.obj;,\
		radar.obj;,\
		readdump.obj;,\
		resource.obj;,\
		satellite.obj;,\
		ship.obj;,\
		special.obj;,\
		spy.obj;,\
		uprintf.obj;,\
		ve.obj;

SRCS	= 	census.c;,\
		coastwatch.c;,\
		commodity.c;,\
		cutoff.c;,\
		dump.c;,\
		getopt.c;,\
		level.c;,\
		lookout.c;,\
		map.c;,\
		misc.c;,\
		plane.c;,\
		profile.c;,\
		radar.c;,\
		readdump.c;,\
		resource.c;,\
		satellite.c;,\
		ship.c;,\
		special.c;,\
		spy.c;,\
		uprintf.c;,\
		ve.c;

DIST	= README $(SRCS) ve.h Makefile ve.6

#################################################################
#
# Rules.
#
#################################################################

ve: $(OBJS)
	$(LINK) $(LDFLAGS) $(OBJS) $(LIBS)

debug: $(SRCS)
	$(CC) $(CFLAGS) /noop/deb $(SRCS)
	$(LINK) /exe=$(EXE)/debug $(OBJS) $(LIBS)

clean: tidy
tidy:
	rm $(OBJS)

clobber: tidy
	rm ve tags lint.out core a.out gmon.out

install: ve
	cp ve.exe empbin

ci:	$(DIST)
	ci $(CIFLAGS) $?

co:
	co $(COFLAGS) $(DIST)

#################################################################
#
# Dependencies
#
#################################################################

$(OBJS): ve.h
