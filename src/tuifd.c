#include "init.h"
#include "layout.h"
#include "options.h"
#include "search.h"
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define RESULT_WINDOW_ID 2
#define OPTIONS_WINDOW_ID 1
#define SEARCH_WINDOW_ID 0
#define TOTAL_WINDOWS 3

/**
 * TUI 文件搜索程序主函数
 * 使用 ncurses 库创建基于终端的用户界面
 * 按 Tab 键循环切换窗口
 * 按 F1 键退出程序
 */
int main() {
  setlocale(LC_ALL, ""); // 设置本地化，支持中文显示

  // 初始化 ncurses
  init_ncurses();
  init_colors();
  setup_input();

  // 创建窗口布局
  Windows wins = draw_windows();

  // 初始化选项
  Options options = init_options();

  // 显示初始选项
  display_options(wins.options_win, &options);

  char search_buffer[1000] = ""; // 搜索输入缓冲区
  char **search_results = NULL;  // 搜索结果数组
  int result_count = 0;          // 结果数量
  int selected_result = 0;       // 当前选中的结果索引
  int ch;                        // 用户输入字符
  int current_window = SEARCH_WINDOW_ID; // 当前活动窗口

  // 主循环 - 监听用户输入
  while ((ch = getch()) != KEY_F(1)) {
    if (ch == '\t') { // Tab 键循环切换窗口
      // 切换到下一个窗口
      current_window = (current_window + 1) % TOTAL_WINDOWS;

      // 根据当前窗口更新显示
      switch(current_window) {
        case SEARCH_WINDOW_ID:
          // 搜索窗口 - 高亮搜索窗口
          wattron(wins.search_win, A_BOLD);
          wrefresh(wins.search_win);
          wattroff(wins.search_win, A_BOLD);
          break;

        case OPTIONS_WINDOW_ID:
          // 选项窗口 - 高亮选项窗口
          wattron(wins.options_win, A_BOLD);
          display_options(wins.options_win, &options);
          wattroff(wins.options_win, A_BOLD);
          break;

        case RESULT_WINDOW_ID:
          // 结果窗口 - 高亮结果窗口
          wattron(wins.results_win, A_BOLD);
          wrefresh(wins.results_win);
          wattroff(wins.results_win, A_BOLD);
          break;
      }
    } else {
      // 处理当前活动窗口的输入
      switch(current_window) {
        case SEARCH_WINDOW_ID:
          // 搜索窗口 - 处理搜索输入
          if (ch == '\n' || ch == KEY_ENTER) {
            // 清空之前的搜索内容
            strcpy(search_buffer, "");

            // 处理搜索输入
            handle_input(wins.search_win, search_buffer);

            // 如果有输入内容，执行搜索
            if (strlen(search_buffer) > 0) {
              // 释放之前的结果内存（如果有）
              if (search_results != NULL) {
                for (int i = 0; i < result_count; i++) {
                  free(search_results[i]);
                }
                free(search_results);
              }

              // 构建搜索选项
              char *search_options = build_search_options(&options);

              // 获取搜索结果
              search_results = getoutput(search_options, search_buffer, ".", &result_count);

              // 显示结果
              wclear(wins.results_win);
              box(wins.results_win, 0, 0);
              mvwprintw(wins.results_win, 0, 2, " Results ");

              if (search_results != NULL && result_count > 0) {
                // 使用选项中的结果限制
                int display_limit = (result_count < options.result_limit)
                                        ? result_count
                                        : options.result_limit;
                for (int i = 0; i < display_limit; i++) {
                  mvwprintw(wins.results_win, i + 1, 1, "%s", search_results[i]);
                }
              } else {
                mvwprintw(wins.results_win, 1, 1, "No results found");
              }
              wrefresh(wins.results_win);
            }
          }
          break;

        case OPTIONS_WINDOW_ID:
          // 选项窗口 - 处理选项输入
          handle_options_input(wins.options_win, &options, ch);
          display_options(wins.options_win, &options);
          break;

        case RESULT_WINDOW_ID:
          // 结果窗口 - 处理结果导航和选择
          if (ch == KEY_UP) {
            // 向上选择结果
            if (selected_result > 0) {
              selected_result--;
            }
            display_results(wins.results_win, search_results, result_count, selected_result, &options);
          } else if (ch == KEY_DOWN) {
            // 向下选择结果
            if (selected_result < result_count - 1 && selected_result < options.result_limit - 1) {
              selected_result++;
            }
            display_results(wins.results_win, search_results, result_count, selected_result, &options);
          } else if (ch == '\n' || ch == KEY_ENTER) {
            // 打开选中的文件/目录
            if (search_results != NULL && result_count > 0 && selected_result < result_count) {
              open_selected_file(search_results[selected_result]);
            }
          }
          break;
      }
    }
  }

  // 清理内存
  if (search_results != NULL) {
    for (int i = 0; i < result_count; i++) {
      free(search_results[i]);
    }
    free(search_results);
  }
  cleanup_options(&options);

  endwin(); // 结束 ncurses 模式
  return 0;
}
