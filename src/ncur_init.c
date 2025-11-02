#include "init.h"
#include <ncurses.h>

/**
 * 设置输入模式
 * 启用原始输入模式，禁用回显，启用特殊按键
 */
void setup_input() {
  raw();
  noecho();             // 无回显
  keypad(stdscr, TRUE); // 启用箭头键
}

/**
 * 初始化颜色系统
 * 检查终端颜色支持并设置颜色对
 */
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

/**
 * 初始化 ncurses 系统
 * 创建标准屏幕窗口
 */
WINDOW *init_ncurses() {
  WINDOW *win = initscr();
  if (!win) {
    fprintf(stderr, "Error: initscr failed\n");
    exit(1);
  }
  return win;
}

/**
 * 获取终端尺寸
 * 返回终端的行数和列数
 */
void get_terminal_dimensions(int *rows, int *cols) {
  getmaxyx(stdscr, *rows, *cols);
}
