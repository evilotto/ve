EXE = ve
CFLAGS = -g
# CFLAGS  = 
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
		ve.o \
		screens.o \
		$(END)

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
		ve.c \
		screens.c \
		xtn-atlast.c \
		xtn-ficl.c \
		$(END)

XTN = FICL
default: ve

### ATLAST 
ifeq ($(XTN),ATLAST)
ATLAST_SRC = atlast/atlast-64/atlast.c
ATL_FLAGS = -Iatlast/atlast-64/ -DEXPORT -DREADONLYSTRINGS
CFLAGS += ${ATL_FLAGS}
OBJS += xtn-atlast.o 
LDLIBS += -lm atlast/atlast-64/atlast.o 
${ATLAST_SRC}:
	git clone https://github.com/Fourmilab/atlast.git
else ifeq ($(XTN),FICL)

### ficl
CFLAGS += -Ificl/
LDFLAGS = -Lficl
LDLIBS += -lm -lficl
OBJS += xtn-ficl.o 
endif

DIST	= README $(SRCS) ve.h Makefile ve.6

SCREENS = help.raw 

#################################################################
#
# Rules.
#
#################################################################

ve: $(OBJS)

clean: tidy
tidy:
	rm $(OBJS)

tags: $(SRCS) ve.h
	ctags ${SRCS} ve.h 

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

screens.c: $(SCREENS)
	awk -f screens.awk $^ > $@


$(OBJS): ve.h
