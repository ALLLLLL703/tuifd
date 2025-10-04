#include "init.h"
#include <ncurses.h>
void setup_input() {
  raw();
  noecho();             // 无回显
  keypad(stdscr, TRUE); // 启用箭头键
}

void init_colors() {
  if (has_colors() == FALSE) {
    endwin();
    fprintf(stderr, "Terminal does not support colors\n");
    exit(1);
  }
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK); // 背景/结果区：白字黑底
  init_pair(2, COLOR_WHITE, COLOR_BLUE);  // 选项区：白字蓝底
  init_pair(3, COLOR_BLACK, COLOR_GREEN); // 搜索栏：黑字绿底
  bkgd(COLOR_PAIR(1));                    // 设置 stdscr 背景
}
WINDOW *init_ncurses() {
  WINDOW *win = initscr();
  if (!win) {
    fprintf(stderr, "Error: initscr failed\n");
    exit(1);
  }
  return win;
}
void get_terminal_dimensions(int *rows, int *cols) {
  getmaxyx(stdscr, *rows, *cols);
}
