/* abstraction for extension */

#include "ve.h"
#include "xtn.h"
#include "ficl.h"
#include <stdio.h>
#include <stdlib.h>

int *xtn_x, *xtn_y;
Sector *xtn_sect;
char pbuffer[100];

ficlSystem *ficl;
ficlVm *vm;

#define Sl(x) FICL_STACK_CHECK(vm->dataStack, x, 0)
#define So(x) FICL_STACK_CHECK(vm->dataStack, 0, x)
#define S0 (vm->dataStack->top[0].i)
#define S1 (vm->dataStack->top[-1].i)
#define S2 (vm->dataStack->top[-2].i)
#define Push (*++(vm->dataStack->top)).i
#define Npop(n) (vm->dataStack->top -= n)

#define INITIAL_BINDING_SIZE 16
typedef struct binding {
	int key;
	ficlWord *dw;
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
xtn_bind(int key, ficlWord *dw)
{
	Binding *cur = xtn_blookup(key);
	if (cur != NULL) {
		cur->dw = dw;
	} else {
		if (++Bcount >= Bsize) {
			Bsize *= 2;
			bindings = realloc(bindings, Bsize * sizeof(Binding));
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
		int stat = ficlVmExecuteXT(vm, b->dw);
		/*
		 * switch (stat) {
			case ATL_SNORM: break;
			case ATL_UNDEFINED:
				xtn_error("unde"); break;
			default:
				xtn_error("unknown"); break;
				// fprintf(stderr, "%s\n", pbuffer);
				// xtn_error("Unspecified error"); break;
		}
		*/
		mapdr(surmap);
		xtn_getxy(x, y);
		return 1;
	} else {
		return 0;
	}
}

void prim_bind(ficlVm *vm)
{
	/* key word --  */
	Sl(2);
	xtn_bind((int)S1, (ficlWord *)S0);
	Npop(2);
}
void xtn_clrovl(ficlVm *vm)
{
	clearovl();
}

void xtn_setovl(ficlVm *vm)
{
	/* x y color-index -- */
	Sl(3);
	/* range-check index? */
	setovl(S2, S1, COLOR_PAIR(S0));
	Npop(3);
}
void color_pair()
{
	/* color-index foreground background */
	Sl(3);
	init_pair(S2, S1, S0);
	Npop(3);
}

void push_red()
{
	So(1);
	Push = COLOR_PAIR(NCOLOR_ENEMY);
}

void load_sect()
{
	Sl(2);
	if (map[xoffset(S1)][yoffset(S0)] == NULL) {
		memset(xtn_sect, 0, sizeof(Sector));
	} else {
		memcpy(xtn_sect, map[xoffset(S1)][yoffset(S0)], sizeof(Sector));
	}
	Npop(2);
}

int
xtn_init()
{
	int status;
	ficl = ficlSystemCreate(NULL);
	vm = ficlSystemCreateVm(ficl);

	status = ficlVmEvaluate(vm, "variable x");
	xtn_x = (int *)ficlSystemLookup(ficl, "x")->param;
	status = ficlVmEvaluate(vm, "variable y y");
	xtn_y = (int *)ficlSystemLookup(ficl, "y")->param;

	status = ficlVmEvaluate(vm, "variable sector sector");
	ficlVmDictionaryAllot(vm, ficl->dictionary, sizeof(Sector));
	xtn_sect = (Sector *)ficlSystemLookup(ficl, "sector")->param;
	
	Bsize = INITIAL_BINDING_SIZE;
	bindings = calloc(sizeof(Binding), INITIAL_BINDING_SIZE);
#define AddWord(name, code) ficlDictionaryAppendPrimitive(ficl->dictionary, name, code, FICL_WORD_DEFAULT);
	AddWord("clrovl", xtn_clrovl)
	AddWord("setovl", xtn_setovl);
	AddWord("red", push_red);
	AddWord("color_pair", color_pair);
	AddWord("bind", prim_bind);
	AddWord("load-sect", load_sect);
#undef AddWord
	/*
	atl_primdef(xtn_fns);
	init = fopen("init.atl", "r");
	if (init != NULL) {
		int stat = atl_load(init);
		if (stat != ATL_SNORM) {
			fprintf(stderr, "error loading init.atl: %s\n", pbuffer);
			exit(1);
		}
		fclose(init);
	}
	*/
	return 0;
};

void
xtn_setxy(int x, int y)
{
	*(int *) xtn_x = x;
	*(int *) xtn_y = y;
}
void
xtn_getxy(int *x, int *y)
{
	*x = *(int *) xtn_x;
	*y = *(int *) xtn_y;
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
/*
int
printf(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	const int ret = vsnprintf(pbuffer, 100, fmt, argp);
	va_end(argp);
	output = 1;
	return ret;
}
*/
	
int
xtn_eval(const char *cmd)
{
	ficlVmEvaluate(vm, (char *)cmd);
	if (output) {
		xtn_error(pbuffer);
		output = 0;
	}
	return 0;
}
