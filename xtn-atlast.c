/* abstraction for extension */

#include "ve.h"
#include "xtn.h"
#include "atldef.h"
#include <stdio.h>
#include <stdlib.h>

dictword *xtn_x, *xtn_y;
dictword *xtn_sect;
char pbuffer[100];

#define INITIAL_BINDING_SIZE 16
typedef struct binding {
	int key;
	dictword *dw;
} Binding;
static Binding *bindings;
static int Bcount = 0, Bsize;

int bind_compar(const void *pa, const void *pb)
{
	Binding *a = (Binding *)pa;
	Binding *b = (Binding *)pb;
	return a->key < b->key ? -1 : a->key > b->key;
}

Binding*
xtn_blookup(int key)
{
	Binding search = {key, NULL};
	return (Binding *)bsearch(&search, bindings,
		Bcount, sizeof(Binding), bind_compar);
}

void
xtn_bind(int key, dictword *dw)
{
	Binding *cur = xtn_blookup(key);
	if (cur != NULL) {
		cur->dw = dw;
	} else {
		if (++Bcount >= Bsize) {
			void *_b = bindings;
			Bsize *= 2;
			if ((_b = realloc(_b, Bsize * sizeof(Binding))) != NULL) {
				bindings = _b;
			}
			// bindings = realloc(bindings, Bsize * sizeof(Binding));
		}
		(bindings + Bcount-1)->key = key;
		(bindings + Bcount-1)->dw = dw;
		qsort(bindings, Bcount, sizeof(Binding), bind_compar);
	}
}

int
xtn_execbind(int key, int *x, int *y)
{
	Binding *b = xtn_blookup(key);
	if (b != NULL) {
		xtn_setxy(*x, *y);
		int stat = atl_exec(b->dw);
		switch (stat) {
			case ATL_SNORM: break;
			case ATL_UNDEFINED:
				xtn_error("unde"); break;
			default:
				xtn_error("unknown"); break;
				// fprintf(stderr, "%s\n", pbuffer);
				// xtn_error("Unspecified error"); break;
		}
		mapdr(surmap);
		xtn_getxy(x, y);
		return 1;
	} else {
		return 0;
	}
}

prim prim_bind()
{
	/* key word --  */
	Sl(2);
	xtn_bind((int)S1, (dictword *)S0);
	Npop(2);
}
prim xtn_clrovl()
{
	clearovl();
}

prim xtn_setovl()
{
	/* x y color-index -- */
	Sl(3);
	/* range-check index? */
	setovl(S2, S1, COLOR_PAIR(S0));
	Npop(3);
}
prim color_pair()
{
	/* color-index foreground background */
	Sl(3);
	init_pair(S2, S1, S0);
	Npop(3);
}

prim push_red()
{
	So(1);
	Push = COLOR_PAIR(NCOLOR_ENEMY);
}

prim load_sect()
{
	Sl(2);
	if (map[xoffset(S1)][yoffset(S0)] == NULL) {
		memset(atl_body(xtn_sect), 0, sizeof(Sector));
	} else {
		memcpy(atl_body(xtn_sect), map[xoffset(S1)][yoffset(S0)], sizeof(Sector));
	}
	Npop(2);
}

static struct primfcn xtn_fns[] = {
	{"0CLROVL", xtn_clrovl},
	{"0SETOVL", xtn_setovl},
	{"0RED", push_red},
	{"0COLOR_PAIR", color_pair},
	{"0BIND", prim_bind},
	{"0LOAD-SECT", load_sect},
	{0, (codeptr)0}
};

int
xtn_init()
{
	FILE *init;
	atl_init();
	xtn_x = atl_vardef("x", sizeof(int));
	xtn_y = atl_vardef("y", sizeof(int));
	xtn_sect = atl_vardef("sector", sizeof(Sector));
	atl_primdef(xtn_fns);
	Bsize = INITIAL_BINDING_SIZE;
	bindings = calloc(sizeof(Binding), INITIAL_BINDING_SIZE);
	init = fopen("init.atl", "r");
	if (init != NULL) {
		int stat = atl_load(init);
		if (stat != ATL_SNORM) {
			fprintf(stderr, "error loading init.atl: %s\n", pbuffer);
			exit(1);
		}
		fclose(init);
	}
	return 0;
};

void
xtn_setxy(int x, int y)
{
	*(int *) atl_body(xtn_x) = x;
	*(int *) atl_body(xtn_y) = y;
}
void
xtn_getxy(int *x, int *y)
{
	*x = *(int *) atl_body(xtn_x);
	*y = *(int *) atl_body(xtn_y);
}


void xtn_error(const char *s)
{
	WINDOW *errwin;
	errwin = newwin(3, strlen(s)+2, LINES/2, COLS/2-strlen(s)/2);
	mvwaddstr(errwin, 1, 1, s);
	box(errwin, 0, 0);
	halfdelay(20);
	wgetch(errwin);
	cbreak();
	delwin(errwin);
	mapdr(surmap);
}

static int output = 0;
int
printf(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	const int ret = vsnprintf(pbuffer, 100, fmt, argp);
	va_end(argp);
	output = 1;
	return ret;
}
	
atl_statemark mk;
int
xtn_eval(const char *cmd)
{
	atl_mark(&mk);
	int stat;
	int (*origp)(const char *fmt, ...) = printf;
	// printf = xtn_printf;
	if ((stat = atl_eval(cmd)) != ATL_SNORM) {
		switch (stat) {
			case ATL_UNDEFINED:
				xtn_error(pbuffer); break;
			default:
				xtn_error(pbuffer); break;
				fprintf(stderr, "%s\n", pbuffer);
				// xtn_error("Unspecified error"); break;
		}
		atl_unwind(&mk);
	}
	if (output) {
		xtn_error(pbuffer);
		output = 0;
	}
	// printf = origp;
	return (stat == ATL_SNORM);
}
