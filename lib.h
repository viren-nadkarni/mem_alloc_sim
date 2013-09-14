#ifndef LIB_H
#define LIB_H

#include <ncurses.h>

void print_menu(WINDOW *, char **, int, int);
int menu_control(WINDOW *, char **, int, int);

#endif
