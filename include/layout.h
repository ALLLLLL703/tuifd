#ifndef LAYOUT_H
#define LAYOUT_H

#include <ncurses.h>
#include <stdlib.h>

typedef struct {
		WINDOW *options_win;
		WINDOW *results_win;
		WINDOW *search_win;
} Windows;
// 函数声明
extern Windows* windows;
Windows draw_windows();
WINDOW* draw_search_windows(WINDOW* win);
#endif
