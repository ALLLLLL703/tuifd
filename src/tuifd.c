#include "init.h"
#include "layout.h"
#include "search.h"
#include <ncurses.h>
int main() {
  init_ncurses();
  init_colors();
  setup_input();
  Windows wins = draw_windows();
  int nwin = 0;
  int ch;

  char *a;
  while (ch != KEY_F(1)) {
    ch = getch();
    if (ch == 9) {
      nwin++;
      if (nwin == 1)
        handle_input(wins.search_win, a);
      if (nwin == 2) {
        nwin = 0;

      } else {
      }
    }
  }
  getch();
  endwin();
  return 0;
}
