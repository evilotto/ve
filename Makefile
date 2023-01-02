EXE = ve
# CFLAGS = -g
CFLAGS  = 
LDFLAGS = 
LDLIBS	= -lncurses

OBJS	= 	census.o \
		coastwatch.o \
		commodity.o \
		cutoff.o \
		dump.o \
		level.o \
		lookout.o \
		map.o \
		misc.o \
		plane.o \
		profile.o \
		radar.o \
		readdump.o \
		resource.o \
		satellite.o \
		ship.o \
		special.o \
		spy.o \
		uprintf.o \
		unit.o \
		ve.o

SRCS	= 	census.c \
		coastwatch.c \
		commodity.c \
		cutoff.c \
		dump.c \
		level.c \
		lookout.c \
		map.c \
		misc.c \
		plane.c \
		profile.c \
		radar.c \
		readdump.c \
		resource.c \
		satellite.c \
		ship.c \
		special.c \
		spy.c \
		uprintf.c \
		ve.c

DIST	= README $(SRCS) ve.h Makefile ve.6

#################################################################
#
# Rules.
#
#################################################################

ve: $(OBJS)

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
