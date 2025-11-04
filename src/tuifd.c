#include "cache.h"
#include "init.h"
#include "layout.h"
#include "options.h"
#include "search.h"
#include "tools.h"
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define RESULT_WINDOW_ID 2
#define OPTIONS_WINDOW_ID 1
#define SEARCH_WINDOW_ID 0
#define TOTAL_WINDOWS 3

void init() {
  init_ncurses();
  init_colors();
  setup_input();
}

int motion_count_regist = 0;
/**
 * TUI 文件搜索程序主函数
 * 使用 ncurses 库创建基于终端的用户界面
 * 按 Tab 键循环切换窗口
 * 按 F1 键退出程序
 * 第一个参数作为搜索路径，默认为当前目录
 */
int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); // 设置本地化，支持中文显示

  // 初始化 ncurses
  init();

  // 创建窗口布局
  Windows wins = draw_windows();
  all_windows = wins;
  // 初始化选项
  Options options = init_options();

  // 确定搜索路径
  char search_path[1024] = ".";
  if (argc > 1) {
    strncpy(search_path, argv[1], sizeof(search_path) - 1);
    search_path[sizeof(search_path) - 1] = '\0';
  }

  // 显示初始选项
  display_options(wins.options_win, &options);

  char search_buffer[1000] = "";         // 搜索输入缓冲区
  char **search_results = NULL;          // 搜索结果数组
  int result_count = 0;                  // 结果数量
  int selected_result = 0;               // 当前选中的结果索引
  int scroll_offset = 0;                 // 结果窗口滚动偏移
  int ch;                                // 用户输入字符
  int current_window = SEARCH_WINDOW_ID; // 当前活动窗口
  int search_insert_mode = 1;            // 搜索窗口插入模式标志

  // 主循环 - 监听用户输入
  while ((ch = getch()) != KEY_F(1) && ch != '') {
    if (ch == '\t') { // Tab 键循环切换窗口
      // 切换到下一个窗口
      current_window = (current_window + 1) % TOTAL_WINDOWS;

      // 根据当前窗口更新显示
      switch (current_window) {
      case SEARCH_WINDOW_ID:
        // 搜索窗口 - 高亮搜索窗口并立即进入插入模式
        wattron(wins.search_win, A_BOLD);
        wrefresh(wins.search_win);
        wattroff(wins.search_win, A_BOLD);

        // 立即进入插入模式处理搜索输入
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
      switch (current_window) {
      case SEARCH_WINDOW_ID:
        // 搜索窗口 - 处理搜索输入
        if (ch) {
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
            char *actual_search_path =
                options.search_path ? options.search_path : search_path;
            search_results = getoutput(search_options, search_buffer,
                                       actual_search_path, &result_count);

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
            current_window = RESULT_WINDOW_ID;
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
        if (ch == KEY_UP || ch == 'k') {
          // 向上选择结果
          selected_result -= motion_count_regist >= 1 ? motion_count_regist : 1;
          motion_count_regist = 0;

          if (selected_result < 0) {
            selected_result = 0;
          }
          display_results(wins.results_win, search_results, result_count,
                          selected_result, scroll_offset, &options);
        } else if (ch == KEY_DOWN || ch == 'j') {
          // 向下选择结果
          selected_result += motion_count_regist >= 1 ? motion_count_regist : 1;
          motion_count_regist = 0;
          if (selected_result > result_count - 1 ||
              selected_result > options.result_limit - 1) {
            selected_result = result_count >= options.result_limit
                                  ? options.result_limit - 1
                                  : result_count - 1;
          }
          display_results(wins.results_win, search_results, result_count,
                          selected_result, scroll_offset, &options);
        } else if (ch == KEY_PPAGE || ch == '') {
          // Page Up - 向上滚动
          if (scroll_offset > 0) {
            scroll_offset -= 5; // 每次滚动5行
            if (scroll_offset < 0)
              scroll_offset = 0;
            display_results(wins.results_win, search_results, result_count,
                            selected_result, scroll_offset, &options);
          }
        } else if (ch == KEY_NPAGE || ch == '') {
          // Page Down - 向下滚动
          if (scroll_offset + options.result_limit < result_count) {
            scroll_offset += 5; // 每次滚动5行
            if (scroll_offset + options.result_limit > result_count) {
              scroll_offset = result_count - options.result_limit;
            }
            display_results(wins.results_win, search_results, result_count,
                            selected_result, scroll_offset, &options);
          }
        } else if (ch == '\n' || ch == KEY_ENTER) {
          // 打开选中的文件/目录
          if (search_results != NULL && result_count > 0 &&
              selected_result < result_count) {
            // 保存当前程序状态到缓存
            save_program_state(search_buffer, search_results, result_count,
                               selected_result, scroll_offset, current_window,
                               &options);

            open_selected_file(search_results[selected_result]);

            // 从缓存恢复程序状态
            ProgramState *restored_state = restore_program_state();
            if (restored_state) {
              // 恢复搜索缓冲区
              strcpy(search_buffer, restored_state->search_buffer);

              // 恢复搜索结果
              if (search_results != NULL) {
                for (int i = 0; i < result_count; i++) {
                  free(search_results[i]);
                }
                free(search_results);
              }

              search_results = restored_state->search_results;
              result_count = restored_state->result_count;
              selected_result = restored_state->selected_result;
              current_window = restored_state->current_window;

              // 恢复选项设置
              options = restored_state->options;

              // 清理恢复的状态结构体（不释放搜索结果，因为它们现在被主程序使用）
              free(restored_state);

              // 立即刷新所有窗口内容
              refresh_window(wins.search_win, search_buffer);
              display_options(wins.options_win, &options);
              display_results(wins.results_win, search_results, result_count,
                              selected_result, scroll_offset, &options);
            }
          }
        } else if (ch >= '0' && ch <= '9') {

          ch = ch - '0';
          motion_count_regist = motion_count_regist * 10 + ch;
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
