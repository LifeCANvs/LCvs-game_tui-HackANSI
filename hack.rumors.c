/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.rumors.c - version 1.0.3 */

#include <stdio.h>

#include "hack.h"  /* for RUMORFILE and BSD (index) */

#define CHARSZ	8			/* number of bits in a char */

static int n_rumors = 0;
static int n_used_rumors = -1;
static char *usedbits;

static int skipline(FILE *rumf);
static int used(int i);

static void init_rumors(FILE *rumf) {
	int i;
	n_used_rumors = 0;
	while(skipline(rumf)) n_rumors++;
	rewind(rumf);
	i = n_rumors/CHARSZ;
	usedbits = (char *) alloc((unsigned)(i+1));
	for( ; i>=0; i--) usedbits[i] = 0;
}

static int skipline(FILE *rumf) {
	char line[COLNO];
	while(1) {
		if(!fgets(line, sizeof(line), rumf)) return(0);
		if(index(line, '\n')) return(1);
	}
}

static void outline(FILE *rumf) {
	char line[COLNO];
	char *ep;
	if(!fgets(line, sizeof(line), rumf)) return;
	if((ep = index(line, '\n')) != 0) *ep = 0;
	pline("This cookie has a scrap of paper inside! It reads: ");
	pline(line);
}

void outrumor(void) {
	int rn,i;
	FILE *rumf;
	if(n_rumors <= n_used_rumors ||
	  (rumf = fopen(RUMORFILE, "r")) == (FILE *) 0) return;
	if(n_used_rumors < 0) init_rumors(rumf);
	if(!n_rumors) goto none;
	rn = rn2(n_rumors - n_used_rumors);
	i = 0;
	while(rn || used(i)) {
		(void) skipline(rumf);
		if(!used(i)) rn--;
		i++;
	}
	usedbits[i/CHARSZ] |= (1 << (i % CHARSZ));
	n_used_rumors++;
	outline(rumf);
none:
	(void) fclose(rumf);
}

static int used(int i) {
	return(usedbits[i/CHARSZ] & (1 << (i % CHARSZ)));
}
