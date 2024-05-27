extern int xtn_init();
extern int xtn_eval(const char *cmd);
extern int xtn_execbind(int key, int *x, int *y);
extern void xtn_setxy(int x, int y);
extern void xtn_getxy(int *x, int *y);
extern void xtn_error(const char *err);
