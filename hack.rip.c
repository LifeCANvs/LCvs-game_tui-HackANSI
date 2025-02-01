/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.rip.c - version 1.0.2 */

#include <stdio.h>
#include "hack.h"

static char *rip[] = {
"                       ----------",
"                      /          \\",
"                     /    REST    \\",
"                    /      IN      \\",
"                   /     PEACE      \\",
"                  /                  \\",
"                  |                  |",
"                  |                  |",
"                  |                  |",
"                  |                  |",
"                  |                  |",
"                  |       1001       |",
"                 *|     *  *  *      | *",
"        _________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______\n",
0
};

// Big rewrite here to account for the fact we can't modify constant strings
// these days!  We copy each line to a buffer, then centre a string within it
// on particular lines.

void outrip(void) {
	char *dpx;
	char outbuf[BUFSZ];
	char buf[BUFSZ];
	int x,y;
	int line;
	char *thisline;
	char *line10 = buf;
	char *ip,*op;

	y = 8;

	cls();
	for (line = 0; rip[line]; ++line) {
		strcpy(outbuf, rip[line]);
		strcat(outbuf, "\n");
		buf[0] = 0;
		thisline = buf;
		switch (line) {
		default:
			break;
		case 6:
			(void) strcpy(buf, plname);
			buf[16] = 0;
			break;
		case 7:
			(void) sprintf(buf, "%ld AU", u.ugold);
			break;
		case 8:
			(void) sprintf(buf, "killed by%s",
				       !strncmp(killer, "the ", 4) ? "" :
				       !strcmp(killer, "starvation") ? "" :
				       index(vowels, *killer) ? " an" : " a");
			break;
		case 9:
			(void) strcpy(buf, killer);
			if(strlen(buf) > 16) {
				int i,i0,i1;
				i0 = i1 = 0;
				for(i = 0; i <= 16; i++)
					if(buf[i] == ' ') i0 = i, i1 = i+1;
				if(!i0) i0 = i1 = 16;
				buf[i1 + 16] = 0;
				line10 = buf+i1;
				buf[i0] = 0;
			}
			break;
		case 10:
			thisline = line10;
			break;
		case 11:
			(void) sprintf(buf, "%4d", getyear());
			break;
		}

		ip = buf;
		op = &outbuf[28 - ((strlen(thisline)+1)/2)];
		while(*ip) *op++ = *ip++;

		x = 0;
		dpx = outbuf;
		while(dpx[x]) {
			while(dpx[x] == ' ') x++;
			curs(x,y);
			while(dpx[x] && dpx[x] != ' '){
				if(done_stopprint)
					return;
				curx++;
				(void) putchar(dpx[x++]);
			}
		}
		++y;
	}
	getret();
}
