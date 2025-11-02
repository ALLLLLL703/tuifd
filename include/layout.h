#ifndef LAYOUT_H
#define LAYOUT_H

#include <ncurses.h>
#include <stdlib.h>

/**
 * 窗口布局相关定义
 * 包含窗口结构体定义和窗口管理函数声明
 */

// 窗口结构体，包含三个主要窗口
typedef struct {
    WINDOW *options_win;  // 选项窗口
    WINDOW *results_win;  // 结果窗口
    WINDOW *search_win;   // 搜索窗口
} Windows;

// 函数声明

// 创建并绘制所有窗口
Windows draw_windows(void);

// 绘制搜索窗口
WINDOW* draw_search_windows(WINDOW* win);

#endif
