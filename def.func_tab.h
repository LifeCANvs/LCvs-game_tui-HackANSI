/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* def.func_tab.h - version 1.0.2 */

struct func_tab {
	char f_char;
	int (*f_funct)();
};

struct ext_func_tab {
	char *ef_txt;
	int (*ef_funct)();
};
