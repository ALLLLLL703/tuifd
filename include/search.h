#ifndef SEARCH_H
#define SEARCH_H
#include <string.h>
#include <ncurses.h>
extern WINDOW *search_window;
extern char sum_str[1000];
#endif
WINDOW* refresh_window(WINDOW* win, const char* input);
char* handle_input(WINDOW* win,  char* input);
