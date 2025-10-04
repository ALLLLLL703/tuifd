#include "layout.h"
#include <ncurses.h>
Windows *windows;
Windows draw_windows() {
  WINDOW *options_win, *results_win, *search_win;
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);
  options_win = newwin(max_y - 1, max_x / 4, 0, 0);
  search_win = newwin(3, max_x * 3 / 4, 0, max_x / 4 + 1);
  results_win = newwin(max_y - 3, max_x * 3 / 4, 4, max_x / 4);
  Windows wins = {options_win, results_win, search_win};

  box(options_win, 0, 0);
  box(results_win, 0, 0);
  box(search_win, 0, 0);
  mvwprintw(options_win, 0, 2, " Options ");
  mvwprintw(results_win, 0, 2, " Results ");
  mvwprintw(search_win, 0, 2, " Search ");
  wbkgd(options_win, COLOR_PAIR(2));
  wbkgd(results_win, COLOR_PAIR(1));
  wbkgd(search_win, COLOR_PAIR(3));
  refresh();

  wrefresh(options_win);
  wrefresh(results_win);
  wrefresh(search_win);
  windows = &wins;
  return wins;
}
WINDOW *draw_search_windows(WINDOW *win) {
  box(win, 0, 0);

  mvwprintw(win, 0, 2, " Search ");
  wbkgd(win, COLOR_PAIR(3));
  refresh();
  wrefresh(win);
  return win;
}
