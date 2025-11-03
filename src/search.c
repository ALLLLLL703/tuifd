#include "search.h"
#include "layout.h"
#include <ncurses.h>
#include <string.h>

WINDOW *search_window;

/**
 * 处理搜索输入
 * 在指定窗口中接收用户输入，支持退格键删除
 * 返回输入字符串
 */
char *handle_input(WINDOW *win, char *input_buffer) {
  static char buffer[1000] = ""; // 输入缓冲区
  int ch;

  while ((ch = getch()) != '\n' && ch != KEY_ENTER) {
    if (ch >= 32 && ch <= 126) { // 可打印字符
      size_t len = strlen(buffer);
      if (len < sizeof(buffer) - 1) {
        buffer[len] = (char)ch;
        buffer[len + 1] = '\0';
      }
    } else if (ch == KEY_BACKSPACE || ch == '\b') { // 退格键
      size_t len = strlen(buffer);
      if (len > 0) {
        buffer[len - 1] = '\0';
      }
    } else if (ch == 9) {
      return NULL;
    }

    // 刷新窗口显示
    wclear(win);
    draw_search_windows(win);
    refresh_window(win, buffer);
  }

  // 如果提供了输出缓冲区，将结果复制到其中
  if (input_buffer != NULL) {
    strncpy(input_buffer, buffer, 999);
    input_buffer[999] = '\0';
    return input_buffer;
  }

  return buffer;
}

/**
 * 刷新搜索窗口
 * 更新窗口内容并显示当前输入
 */
WINDOW *refresh_window(WINDOW *win, const char *input) {
  wclear(win);
  draw_search_windows(win);

  int max_x, max_y;
  getmaxyx(win, max_y, max_x);
  (void)max_y; // 标记为故意未使用
  max_x -= 2;
  mvwprintw(win, 1, 1, "%s", input);
  wrefresh(win);
  refresh();
  return win;
}

/**
 * 执行文件搜索命令
 * 使用 fd 命令搜索文件，返回搜索结果数组
 */
char **getoutput(char *option, char *target, char *path, int *line_count) {
  // 为结果数组分配内存
  char **result = malloc(MAX_LINES * sizeof(char *));
  if (!result) {
    return NULL;
  }

  // 安全构建命令字符串
  char full_cmd[BUFFER_SIZE];
  snprintf(full_cmd, sizeof(full_cmd), "%s", CMD);

  if (option && strlen(option) > 0) {
    strncat(full_cmd, " ", sizeof(full_cmd) - strlen(full_cmd) - 1);
    strncat(full_cmd, option, sizeof(full_cmd) - strlen(full_cmd) - 1);
  }

  // 如果目标为空，使用 "." 匹配所有文件
  if (target && strlen(target) > 0) {
    strncat(full_cmd, " ", sizeof(full_cmd) - strlen(full_cmd) - 1);
    strncat(full_cmd, target, sizeof(full_cmd) - strlen(full_cmd) - 1);
  } else {
    strncat(full_cmd, " .", sizeof(full_cmd) - strlen(full_cmd) - 1);
  }

  strncat(full_cmd, " ", sizeof(full_cmd) - strlen(full_cmd) - 1);
  strncat(full_cmd, path, sizeof(full_cmd) - strlen(full_cmd) - 1);

  // 执行命令
  FILE *pipe = popen(full_cmd, "r");
  if (!pipe) {
    free(result);
    return NULL;
  }

  // 逐行读取输出
  int count = 0;
  char buffer[BUFFER_SIZE];

  while (fgets(buffer, sizeof(buffer), pipe) && count < MAX_LINES) {
    // 如果存在换行符，移除它
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }

    // 为行分配内存并复制内容
    result[count] = malloc(strlen(buffer) + 1);
    if (!result[count]) {
      // 分配失败时的清理
      for (int i = 0; i < count; i++) {
        free(result[i]);
      }
      free(result);
      pclose(pipe);
      return NULL;
    }
    strcpy(result[count], buffer);
    count++;
  }

  pclose(pipe);
  *line_count = count;
  return result;
}
