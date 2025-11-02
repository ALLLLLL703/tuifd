#ifndef INIT_H
#define INIT_H

#include <ncurses.h>
#include <stdlib.h>

/**
 * 初始化相关函数声明
 * 包含 ncurses 初始化、颜色设置、输入模式设置等功能
 */

// 初始化 ncurses 系统
WINDOW *init_ncurses(void);

// 设置输入模式
void setup_input(void);

// 初始化颜色系统
void init_colors(void);

// 获取终端尺寸
void get_terminal_dimensions(int *rows, int *cols);

#endif
