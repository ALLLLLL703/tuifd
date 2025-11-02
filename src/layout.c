#include "layout.h"
#include <ncurses.h>
Windows all_windows;
/**
 * 创建并绘制所有窗口
 * 返回包含三个窗口的结构体
 */
Windows draw_windows() {
  WINDOW *options_win, *results_win, *search_win;
  int max_y, max_x;

  // 获取终端尺寸
  getmaxyx(stdscr, max_y, max_x);

  // 创建三个窗口：选项窗口、结果窗口、搜索窗口
  options_win = newwin(max_y - 1, max_x / 4, 0, 0);             // 左侧选项窗口
  search_win = newwin(3, max_x * 3 / 4, 0, max_x / 4 + 1);      // 顶部搜索窗口
  results_win = newwin(max_y - 3, max_x * 3 / 4, 4, max_x / 4); // 主结果窗口

  Windows wins = {options_win, results_win, search_win};

  // 为每个窗口添加边框和标题
  box(options_win, 0, 0);
  box(results_win, 0, 0);
  box(search_win, 0, 0);
  mvwprintw(options_win, 0, 2, " Options ");
  mvwprintw(results_win, 0, 2, " Results ");
  mvwprintw(search_win, 0, 2, " Search ");

  // 设置窗口颜色
  wbkgd(options_win, COLOR_PAIR(2)); // 选项窗口：白字蓝底
  wbkgd(results_win, COLOR_PAIR(1)); // 结果窗口：白字黑底
  wbkgd(search_win, COLOR_PAIR(3));  // 搜索窗口：黑字绿底

  refresh();

  // 刷新所有窗口
  wrefresh(options_win);
  wrefresh(results_win);
  wrefresh(search_win);

  return wins;
}

/**
 * 绘制搜索窗口
 * 为搜索窗口添加边框和标题
 */
WINDOW *draw_search_windows(WINDOW *win) {
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " Search ");
  wbkgd(win, COLOR_PAIR(3)); // 搜索窗口：黑字绿底
  refresh();
  wrefresh(win);
  return win;
}

/**
 * 刷新所有窗口
 * 确保所有窗口内容正确显示
 */
void refresh_all(Windows win) {
  refresh();
  wrefresh(win.options_win);
  wrefresh(win.results_win);
  wrefresh(win.search_win);
}

// 未实现的窗口切换功能
void change_window(Windows win, char *ch);
