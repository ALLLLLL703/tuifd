#ifndef SEARCH_H
#define SEARCH_H

#include <string.h>
#include <ncurses.h>

/**
 * 搜索相关定义
 * 包含搜索命令定义、搜索函数声明和常量定义
 */

#define CMD "/usr/bin/fd"      // 使用的搜索命令
#define MAX_LINES 100000        // 最大搜索结果行数
#define BUFFER_SIZE 1024        // 缓冲区大小

// 函数声明

// 刷新搜索窗口显示
WINDOW* refresh_window(WINDOW* win, const char* input);

// 处理搜索输入
char* handle_input(WINDOW* win, char* input);

// 执行搜索并返回结果
char **getoutput(char *option, char *target, char *path, int *line_count);

#endif
