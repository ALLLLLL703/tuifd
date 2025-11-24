#include "options.h"
#include "layout.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * 初始化选项设置
 * 设置默认的搜索和显示选项
 */
char *search_str[8] = {"[%c] 区分大小写", "[%c] 包含隐藏文件", "深度限制: %d",
                       "结果限制: %d",    "排序方式: %s",      "文件扩展名: %s",
                       "搜索路径: %s",    "文件类型: %s"};

Options init_options() {
  Options opts = {
      .case_sensitive = 0,  // 默认不区分大小写
      .include_hidden = 0,  // 默认不包含隐藏文件
      .max_depth = 0,       // 默认无深度限制
      .result_limit = 2000, // 默认显示20个结果
      .file_ext = NULL,     // 默认无文件类型限制
      .sort_by = SORT_NAME, // 默认按名称排序
      .search_path = NULL,  // 默认搜索路径
      .selected_option = 0, // 默认选中第一个选项
      .file_type = NULL,

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
  for (int i = 0; i < 8; i++) {
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
      printf("%s", search_str[1]);
      break;
    case 4: {
      const char *sort_names[] = {"名称", "大小", "修改时间"};
      mvwprintw(win, y++, 2, "排序方式: %s", sort_names[opts->sort_by]);
    } break;
    case 5:
      if (opts->file_ext) {
        mvwprintw(win, y++, 2, "文件扩展名: %s", opts->file_ext);
      } else {
        mvwprintw(win, y++, 2, "文件扩展名: 所有文件");
      }
      break;
    case 6:
      if (opts->search_path) {
        mvwprintw(win, y++, 2, "搜索路径: %s", opts->search_path);
      } else {
        mvwprintw(win, y++, 2, "搜索路径: 当前目录");
      }
      break;
    case 7:
      if (opts->file_type) {
        mvwprintw(win, y++, 2, "文件类型: %s", opts->file_type);

      } else {

        mvwprintw(win, y++, 2, "文件类型: %s", "all");
      }
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
      if (opts->file_ext) {
        free(opts->file_ext);
      }
      opts->file_ext = strdup(input_buffer);
    } else {
      if (opts->file_ext) {
        free(opts->file_ext);
        opts->file_ext = NULL;
      }
    }
    break;

  case 6: // 搜索路径
    mvwprintw(win, getmaxy(win) - 1, 2, ":");
    wrefresh(win);
    echo();
    wgetnstr(win, input_buffer, sizeof(input_buffer) - 1);
    noecho();
    if (strlen(input_buffer) > 0) {
      if (opts->search_path) {
        free(opts->search_path);
      }
      opts->search_path = strdup(input_buffer);
    } else {
      if (opts->search_path) {
        free(opts->search_path);
        opts->search_path = NULL;
      }
    }
    break;
  case 7:
    mvwprintw(win, getmaxy(win) - 1, 2, ":");
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
  int option_count = 8; // 当前选项数量

  switch (ch) {
  case KEY_UP:
  case 'k':
    opts->selected_option =
        (opts->selected_option - 1 + option_count) % option_count;
    break;
  case KEY_DOWN:
  case 'j':
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

  if (opts->file_ext) {
    char ext_str[50];
    snprintf(ext_str, sizeof(ext_str), " -e %s", opts->file_ext);
    strcat(options, ext_str);
  }
  if (opts->file_type) {
    char type_str[20];
    snprintf(type_str, sizeof(type_str), " -t %s", opts->file_type);
    strcat(options, type_str);
  }

  return options;
}

/**
 * 显示搜索结果
 * 在结果窗口中显示搜索结果，并高亮选中的结果
 */
void display_results(WINDOW *win, char **results, int result_count,
                     int selected, int scroll_offset, Options *opts) {
  wclear(win);
  box(win, 0, 0);
  mvwprintw(win, 0, 2, " Results ");

  if (results != NULL && result_count > 0) {
    // 获取窗口高度
    int win_height, win_width;
    getmaxyx(win, win_height, win_width);
    int max_display_lines = win_height - 2; // 减去边框

    // 使用选项中的结果限制和窗口高度限制
    int display_limit =
        (result_count < opts->result_limit) ? result_count : opts->result_limit;
    display_limit =
        (display_limit < max_display_lines) ? display_limit : max_display_lines;

    // 确保选中项在可见范围内
    if (selected < scroll_offset) {
      scroll_offset = selected;
    } else if (selected >= scroll_offset + display_limit) {
      scroll_offset = selected - display_limit + 1;
    }

    // 显示可见范围内的结果
    for (int i = 0; i < display_limit; i++) {
      int result_index = scroll_offset + i;
      if (result_index < result_count) {
        if (result_index == selected) {
          wattron(win, A_REVERSE); // 高亮选中的结果
        }
        mvwprintw(win, i + 1, 1, "%s", results[result_index]);
        if (result_index == selected) {
          wattroff(win, A_REVERSE); // 取消高亮
        }
      }
    }

    // 显示滚动指示器
    if (scroll_offset > 0) {
      mvwprintw(win, 0, getmaxx(win) - 3, "↑");
    }
    if (scroll_offset + display_limit < result_count) {
      mvwprintw(win, getmaxy(win) - 1, getmaxx(win) - 3, "↓");
    }
  } else {
    mvwprintw(win, 1, 1, "No results found");
  }
  wrefresh(win);
}

/**
 * 打开选中的文件/目录
 * 根据文件类型使用不同的程序打开
 */
void open_selected_file(const char *filepath) {
  char command[1024];
  char file_type[256];

  // 使用 file 命令检测文件类型
  snprintf(command, sizeof(command), "file -b \"%s\"", filepath);
  FILE *fp = popen(command, "r");
  if (fp) {
    if (fgets(file_type, sizeof(file_type), fp)) {
      // 移除换行符
      size_t len = strlen(file_type);
      if (len > 0 && file_type[len - 1] == '\n') {
        file_type[len - 1] = '\0';
      }
    }
    pclose(fp);
  }

  // 保存当前终端状态

  // 根据文件类型选择打开方式
  // 目录 - 使用 yazi 打开
  if (strstr(file_type, "directory") != NULL) {
    def_prog_mode();
    endwin();
    printf("Opening directory with yazi: %s\n", filepath);
    snprintf(command, sizeof(command), "yazi \"%s\"", filepath);

    // 使用 fork 创建子进程运行 yazi
    // 父进程继续，不等待子进程
    // 子进程会在后台运行，不会阻塞 TUI
    // fork 失败，回退到原来的方法
    system(command);
  } else if (strstr(file_type, "text") != NULL ||
             strstr(file_type, "ASCII") != NULL ||
             strstr(file_type, "UTF-8") != NULL) {
    // 文本文件 - 使用 nvim 打开
    def_prog_mode();
    endwin();
    printf("Opening text file with nvim: %s\n", filepath);
    snprintf(command, sizeof(command), "nvim \"%s\"", filepath);

    system(command);
  } else if (access(filepath, X_OK) == 0) {
    // 可执行文件 - 同步执行
    def_prog_mode();
    endwin();

    printf("Executing: %s\n", filepath);
    snprintf(command, sizeof(command), "\"%s\"", filepath);
    system(command);
  } else if (strstr(filepath, "microsoft.portable") != NULL) {
    def_prog_mode();
    endwin();
    printf("Executing microsoft portable: %s\n", filepath);
    snprintf(command, sizeof(command), "wine %s", filepath);
    system(command);
  } else {
    printf("Opening with xdg-open: %s\n", filepath);
    snprintf(command, sizeof(command), "nohup xdg-open \"%s\" &>/dev/null &",
             filepath);
    system(command);
  }

  // 恢复 ncurses 模式
  reset_prog_mode();

  // 完全重绘所有窗口
  wclear(all_windows.options_win);
  wclear(all_windows.results_win);
  wclear(all_windows.search_win);

  // 重新绘制窗口边框和标题
  box(all_windows.options_win, 0, 0);
  box(all_windows.results_win, 0, 0);
  box(all_windows.search_win, 0, 0);
  mvwprintw(all_windows.options_win, 0, 2, " Options ");
  mvwprintw(all_windows.results_win, 0, 2, " Results ");
  mvwprintw(all_windows.search_win, 0, 2, " Search ");

  // 重新设置窗口颜色
  wbkgd(all_windows.options_win, COLOR_PAIR(2));
  wbkgd(all_windows.results_win, COLOR_PAIR(1));
  wbkgd(all_windows.search_win, COLOR_PAIR(3));

xdg_open:
  // 刷新所有窗口
  refresh_all(all_windows);
  refresh();
}

/**
 * 清理选项内存
 * 释放选项结构体中分配的内存
 */
void cleanup_options(Options *opts) {
  if (opts->file_ext) {
    free(opts->file_ext);
  }
  if (opts->search_path) {
    free(opts->search_path);
  }
}
