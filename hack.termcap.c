/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.termcap.c - version 1.0.3 */

#include <stdlib.h>
#include <stdio.h>
#include <termcap.h>

#include "hack.h"

static char tbuf[512];
static char *HO, *CL, *CE, *tc_UP, *CM, *ND, *XD, *tc_BC, *SO, *SE, *TI, *TE;
static char *VS, *VE;
static int SG;
static char tc_PC = '\0';
char *CD;		/* tested in pri.c: docorner() */
int CO, LI;		/* used in pri.c and whatis.c */

static void nocmov(int x, int y);
static void cmov(int x, int y);
static int xputc(int c);
static void xputs(char *s);

void startup(void) {
	char *term;
	char *tptr;
	char *tbufptr, *pc;

	tptr = (char *) alloc(1024);

	tbufptr = tbuf;
	if(!(term = getenv("TERM")))
		error("Can't get TERM.");
	if(!strncmp(term, "5620", 4))
		flags.nonull = 1;	/* this should be a termcap flag */
	if(tgetent(tptr, term) < 1)
		error("Unknown terminal type: %s.", term);
	if((pc = tgetstr("pc", &tbufptr)))
		tc_PC = *pc;
	if(!(tc_BC = tgetstr("bc", &tbufptr))) {	
		if(!tgetflag("bs"))
			error("Terminal must backspace.");
		tc_BC = tbufptr;
		tbufptr += 2;
		*tc_BC = '\b';
	}
	HO = tgetstr("ho", &tbufptr);
	CO = tgetnum("co");
	LI = tgetnum("li");
	if(CO < COLNO || LI < ROWNO+2)
		setclipped();
	if(!(CL = tgetstr("cl", &tbufptr)))
		error("Hack needs CL.");
	ND = tgetstr("nd", &tbufptr);
	if(tgetflag("os"))
		error("Hack can't have OS.");
	CE = tgetstr("ce", &tbufptr);
	tc_UP = tgetstr("up", &tbufptr);
	/* It seems that xd is no longer supported, and we should use
	   a linefeed instead; unfortunately this requires resetting
	   CRMOD, and many output routines will have to be modified
	   slightly. Let's leave that till the next release. */
	XD = tgetstr("xd", &tbufptr);
/* not: 		XD = tgetstr("do", &tbufptr); */
	if(!(CM = tgetstr("cm", &tbufptr))) {
		if(!tc_UP && !HO)
			error("Hack needs CM or UP or HO.");
		printf("Playing hack on terminals without cm is suspect...\n");
		getret();
	}
	SO = tgetstr("so", &tbufptr);
	SE = tgetstr("se", &tbufptr);
	SG = tgetnum("sg");	/* -1: not fnd; else # of spaces left by so */
	if(!SO || !SE || (SG > 0)) SO = SE = 0;
	CD = tgetstr("cd", &tbufptr);
	set_whole_screen();		/* uses LI and CD */
	if(tbufptr-tbuf > sizeof(tbuf)) error("TERMCAP entry too big...\n");
	free(tptr);
}

void start_screen(void) {
	xputs(TI);
	xputs(VS);
}

void end_screen(void) {
	xputs(VE);
	xputs(TE);
}

/* Cursor movements */

void curs(int x, int y) {
	/* not xchar: perhaps xchar is unsigned and
	   curx-x would be unsigned as well */

	if (y == cury && x == curx)
		return;
	if(!ND && (curx != x || x <= 3)) {	/* Extremely primitive */
		cmov(x, y);			/* bunker!wtm */
		return;
	}
	if(abs(cury-y) <= 3 && abs(curx-x) <= 3)
		nocmov(x, y);
	else if((x <= 3 && abs(cury-y)<= 3) || (!CM && x<abs(curx-x))) {
		(void) putchar('\r');
		curx = 1;
		nocmov(x, y);
	} else if(!CM) {
		nocmov(x, y);
	} else
		cmov(x, y);
}

static void nocmov(int x, int y) {
	if (cury > y) {
		if(tc_UP) {
			while (cury > y) {	/* Go up. */
				xputs(tc_UP);
				cury--;
			}
		} else if(CM) {
			cmov(x, y);
		} else if(HO) {
			home();
			curs(x, y);
		} /* else impossible("..."); */
	} else if (cury < y) {
		if(XD) {
			while(cury < y) {
				xputs(XD);
				cury++;
			}
		} else if(CM) {
			cmov(x, y);
		} else {
			while(cury < y) {
				xputc('\n');
				curx = 1;
				cury++;
			}
		}
	}
	if (curx < x) {		/* Go to the right. */
		if(!ND) cmov(x, y); else	/* bah */
			/* should instead print what is there already */
		while (curx < x) {
			xputs(ND);
			curx++;
		}
	} else if (curx > x) {
		while (curx > x) {	/* Go to the left. */
			xputs(tc_BC);
			curx--;
		}
	}
}

static void cmov(int x, int y) {
	xputs(tgoto(CM, x-1, y-1));
	cury = y;
	curx = x;
}

static int xputc(int c) {
	(void) fputc(c, stdout);
}

static void xputs(char *s) {
	tputs(s, 1, xputc);
}

void cl_end(void) {
	if(CE)
		xputs(CE);
	else {	/* no-CE fix - free after Harold Rynes */
		/* this looks terrible, especially on a slow terminal
		   but is better than nothing */
		int cx = curx, cy = cury;

		while(curx < COLNO) {
			xputc(' ');
			curx++;
		}
		curs(cx, cy);
	}
}

void clear_screen(void) {
	xputs(CL);
	curx = cury = 1;
}

void home(void) {
	if(HO)
		xputs(HO);
	else if(CM)
		xputs(tgoto(CM, 0, 0));
	else
		curs(1, 1);	/* using UP ... */
	curx = cury = 1;
}

void standoutbeg(void) {
	if(SO) xputs(SO);
}

void standoutend(void) {
	if(SE) xputs(SE);
}

void backsp(void) {
	xputs(tc_BC);
	curx--;
}

void bell(void) {
	(void) putchar('\007');		/* curx does not change */
	(void) fflush(stdout);
}

static short tmspc10[] = {		/* from termcap */
	0, 2000, 1333, 909, 743, 666, 500, 333, 166, 83, 55, 41, 20, 10, 5
};

void delay_output(void) {
	/* delay 50 ms - could also use a 'nap'-system call */
	/* BUG: if the padding character is visible, as it is on the 5620
	   then this looks terrible. */
	if(!flags.nonull)
		tputs("50", 1, xputc);

		/* cbosgd!cbcephus!pds for SYS V R2 */
		/* is this terminfo, or what? */
		/* tputs("$<50>", 1, xputc); */

#ifdef TC_OSPEED
	else if(ospeed > 0 || ospeed < SIZE(tmspc10)) if(CM) {
		/* delay by sending cm(here) an appropriate number of times */
		int cmlen = strlen(tgoto(CM, curx-1, cury-1));
		int i = 500 + tmspc10[ospeed]/2;

		while(i > 0) {
			cmov(curx, cury);
			i -= cmlen*tmspc10[ospeed];
		}
	}
#endif
}

void cl_eos(void) {
	/* free after Robert Viduya */
	/* must only be called with curx = 1 */

	if(CD)
		xputs(CD);
	else {
		int cx = curx, cy = cury;
		while(cury <= LI-2) {
			cl_end();
			xputc('\n');
			curx = 1;
			cury++;
		}
		cl_end();
		curs(cx, cy);
	}
}
