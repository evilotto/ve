/**
 * slist - selection list support
 */

#include "ve.h"
#include <malloc.h>

Slist *mkslist() {
	Slist *s = calloc(sizeof(Slist), 1);
	return s;
}

void slist_add(Slist *s, const char *k, const char *v) {
	Sitem *it = (Sitem *)malloc(sizeof(Sitem));
	if (k) it->key = strdup(k); else it->key = NULL;
	it->val = strdup(v);
	s->entries = realloc(s->entries, sizeof(Sitem*)*(s->n+1));
	*((s->entries)+(s->n)) = it;
	s->n++;
}

void slist_free(Slist *s) {
	int i;
	for (i=0; i< s->n; i++) {
		Sitem *it = slist_item(s, i);
		free(it->key);
		free(it->val);
		free(it);
	}
	free(s->entries);
	free(s);
}

Sitem *slist_item(const Slist *s, int n) {
	return *(s->entries + n);
}
