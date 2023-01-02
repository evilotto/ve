EXE = ve
CFLAGS  = /define=VMS
LINK = link
LDFLAGS = /exe=$(EXE)
LIBS	= ,sys$library:vaxccurse/lib,sys$library:vaxcrtl/lib
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
		unit.obj;,\
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
		unit.c;\
		uprintf.c;,\
		ve.c;

DIST	= README $(SRCS) ve.h Makefile ve.6

#################################################################
#
# Rules.
#
#################################################################

ve : $(OBJS)
	$(LINK) $(LDFLAGS) $(OBJS) $(LIBS)

clean : tidy
tidy :
	rm $(OBJS)

clobber : tidy
	rm ve tags lint.out core a.out gmon.out

install : ve
	cp ve.exe empbin

ci :	$(DIST)
	ci $(CIFLAGS) $?

co :
	co $(COFLAGS) $(DIST)

#################################################################
#
# Dependencies
#
#################################################################

CENSUS.OBJ : ve.h
COASTWATCH.OBJ : ve.h
COMMODITY.OBJ : ve.h
CUTOFF.OBJ : ve.h
DUMP.OBJ : ve.h
GETOPT.OBJ : ve.h
LEVEL.OBJ : ve.h
LOOKOUT.OBJ : ve.h
MAP.OBJ : ve.h
MISC.OBJ : ve.h
PLANE.OBJ : ve.h
PROFILE.OBJ : ve.h
RADAR.OBJ : ve.h
READDUMP.OBJ : ve.h
RESOURCE.OBJ : ve.h
SATELLITE.OBJ : ve.h
SHIP.OBJ : ve.h
SPECIAL.OBJ : ve.h
SPY.OBJ : ve.h
UPRINTF.OBJ : ve.h
VE.OBJ : ve.h
