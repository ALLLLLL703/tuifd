#include "search.h"
#include "layout.h"
#include <ncurses.h>
WINDOW *search_window;
char sum_str[1000];
char *handle_input(WINDOW *win, char *str) {
  int w;
  while (w != KEY_ENTER) {
    w = getch();
    if (w <= 126 && w >= 26) {
      char buffer[1];
      buffer[0] = w;
      strcat(sum_str, buffer);
    }
    if (w == KEY_BACKSPACE) {
      int a = strlen(sum_str);
      sum_str[a - 1] = 0;
    }
    if (w == KEY_F(3)) {
      wmove(windows->results_win, 1, 1);
      break;
    }
    wclear(win);
    draw_search_windows(win);
    wrefresh(win);
    refresh();
    refresh_window(win, sum_str);
  }
  return sum_str;
}
WINDOW *refresh_window(WINDOW *win, const char *input) {
  wclear(win);
  draw_search_windows(win);

  int max_x, max_y;
  getmaxyx(win, max_y, max_x);
  max_x -= 2;
  mvwprintw(win, 1, 1, "%s", input);
  wrefresh(win);
  refresh();
  return win;
}
