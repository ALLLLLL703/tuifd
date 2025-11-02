#include "options.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

/**
 * 初始化选项设置
 * 设置默认的搜索和显示选项
 */
char *search_str[6] = {"[%c] 区分大小写", "[%c] 包含隐藏文件", "深度限制: %d",
                       "结果限制: %d",    "排序方式: %s",      "排序方式: %s"};

Options init_options() {
  Options opts = {
      .case_sensitive = 0,  // 默认不区分大小写
      .include_hidden = 0,  // 默认不包含隐藏文件
      .max_depth = 0,       // 默认无深度限制
      .result_limit = 20,   // 默认显示20个结果
      .file_type = NULL,    // 默认无文件类型限制
      .sort_by = SORT_NAME, // 默认按名称排序
      .selected_option = 0  // 默认选中第一个选项
  };
  return opts;
}

/**
 * 显示选项窗口内容
 * 在选项窗口中显示所有可配置的选项
 */
void display_options(WINDOW *win, Options *opts) {
  wclear(win);
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " Options ");

  // 显示选项列表
  int y = 1;

  // 搜索选项
  for (int i = 0; i < 6; i++) {
    if (i == opts->selected_option) {
      wattron(win, A_REVERSE); // 高亮选中的选项
    }

    switch (i) {
    case 0:
      mvwprintw(win, y++, 2, "[%c] 区分大小写",
                opts->case_sensitive ? 'X' : ' ');
      break;
    case 1:
      mvwprintw(win, y++, 2, "[%c] 包含隐藏文件",
                opts->include_hidden ? 'X' : ' ');
      break;
    case 2:
      mvwprintw(win, y++, 2, "深度限制: %d", opts->max_depth);
      break;
    case 3:
      mvwprintw(win, y++, 2, "结果限制: %d", opts->result_limit);
      break;
    case 4: {
      const char *sort_names[] = {"名称", "大小", "修改时间"};
      mvwprintw(win, y++, 2, "排序方式: %s", sort_names[opts->sort_by]);
    } break;
    case 5:
      if (opts->file_type) {
        mvwprintw(win, y++, 2, "文件类型: %s", opts->file_type);
      } else {
        mvwprintw(win, y++, 2, "文件类型: 所有文件");
      }
      break;
    }

    if (i == opts->selected_option) {
      wattroff(win, A_REVERSE); // 取消高亮
    }
  }

  // 帮助信息
  y = getmaxy(win) - 2;
  mvwprintw(win, y, 2, "↑↓: 导航 空格: 切换 回车: 确认");

  wrefresh(win);
}

/**
 * 处理选项编辑
 * 处理需要输入数值的选项
 */
static void handle_option_edit(WINDOW *win, Options *opts) {
  char input_buffer[100] = "";

  switch (opts->selected_option) {
  case 2: // 深度限制
    mvwprintw(win, getmaxy(win) - 1, 2, "输入深度限制 (0=无限制): ");
    wrefresh(win);
    echo();
    wgetnstr(win, input_buffer, sizeof(input_buffer) - 1);
    noecho();
    opts->max_depth = atoi(input_buffer);
    break;

  case 3: // 结果限制
    mvwprintw(win, getmaxy(win) - 1, 2, "输入结果限制: ");
    wrefresh(win);
    echo();
    wgetnstr(win, input_buffer, sizeof(input_buffer) - 1);
    noecho();
    opts->result_limit = atoi(input_buffer);
    break;

  case 4: // 排序方式
    opts->sort_by = (opts->sort_by + 1) % 3;
    break;

  case 5: // 文件类型
    mvwprintw(win, getmaxy(win) - 1, 2, "输入文件扩展名 (如 .txt): ");
    wrefresh(win);
    echo();
    wgetnstr(win, input_buffer, sizeof(input_buffer) - 1);
    noecho();
    if (strlen(input_buffer) > 0) {
      if (opts->file_type) {
        free(opts->file_type);
      }
      opts->file_type = strdup(input_buffer);
    } else {
      if (opts->file_type) {
        free(opts->file_type);
        opts->file_type = NULL;
      }
    }
    break;
  }
}

/**
 * 处理选项窗口输入
 * 处理用户在选项窗口中的按键输入
 */
void handle_options_input(WINDOW *win, Options *opts, int ch) {
  int option_count = 6; // 当前选项数量

  switch (ch) {
  case KEY_UP:
    opts->selected_option =
        (opts->selected_option - 1 + option_count) % option_count;
    break;
  case KEY_DOWN:
    opts->selected_option = (opts->selected_option + 1) % option_count;
    break;
  case ' ':
    // 切换复选框选项
    switch (opts->selected_option) {
    case 0: // 区分大小写
      opts->case_sensitive = !opts->case_sensitive;
      break;
    case 1: // 包含隐藏文件
      opts->include_hidden = !opts->include_hidden;
      break;
    }
    break;
  case '\n':
  case KEY_ENTER:
    // 处理数值输入选项
    handle_option_edit(win, opts);
    break;
  }
}

/**
 * 构建搜索选项字符串
 * 根据选项设置构建 fd 命令的选项字符串
 */
char *build_search_options(Options *opts) {
  static char options[256] = "";
  options[0] = '\0';

  if (!opts->case_sensitive) {
    strcat(options, " -i");
  }

  if (opts->include_hidden) {
    strcat(options, " -H");
  }

  if (opts->max_depth > 0) {
    char depth_str[20];
    snprintf(depth_str, sizeof(depth_str), " --max-depth %d", opts->max_depth);
    strcat(options, depth_str);
  }

  if (opts->file_type) {
    char type_str[50];
    snprintf(type_str, sizeof(type_str), " -e %s", opts->file_type);
    strcat(options, type_str);
  }

  return options;
}

/**
 * 显示搜索结果
 * 在结果窗口中显示搜索结果，并高亮选中的结果
 */
void display_results(WINDOW *win, char **results, int result_count,
                     int selected, Options *opts) {
  wclear(win);
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " Results ");

  if (results != NULL && result_count > 0) {
    // 使用选项中的结果限制
    int display_limit =
        (result_count < opts->result_limit) ? result_count : opts->result_limit;

    for (int i = 0; i < display_limit; i++) {
      if (i == selected) {
        wattron(win, A_REVERSE); // 高亮选中的结果
      }
      mvwprintw(win, i + 1, 1, "%s", results[i]);
      if (i == selected) {
        wattroff(win, A_REVERSE); // 取消高亮
      }
    }
  } else {
    mvwprintw(win, 1, 1, "No results found");
  }
  wrefresh(win);
}

/**
 * 打开选中的文件/目录
 * 使用 xdg-open 命令打开选中的文件或目录
 */
void open_selected_file(const char *filepath) {
  char command[1024];
  snprintf(command, sizeof(command), "xdg-open \"%s\" 2>/dev/null &", filepath);
  system(command);
}

/**
 * 清理选项内存
 * 释放选项结构体中分配的内存
 */
void cleanup_options(Options *opts) {
  if (opts->file_type) {
    free(opts->file_type);
  }
}
