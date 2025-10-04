#ifndef INIT_H
#define INIT_H

#include <ncurses.h>
#include <stdlib.h>

// 函数声明
WINDOW *init_ncurses(void);
void setup_input(void);
void init_colors(void);
void get_terminal_dimensions(int *rows, int *cols);

#endif
