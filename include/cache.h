#ifndef CACHE_H
#define CACHE_H

#include "options.h"

#define CACHE_DIR "~/.cache/tuifd"
#define CACHE_FILE "~/.cache/tuifd/state.cache"

/**
 * 程序状态结构体
 * 保存需要缓存的所有程序状态
 */
typedef struct {
    char search_buffer[1000];     // 搜索输入
    char **search_results;        // 搜索结果
    int result_count;             // 结果数量
    int selected_result;          // 选中的结果索引
    int scroll_offset;            // 滚动偏移
    int current_window;           // 当前活动窗口
    Options options;              // 选项设置
} ProgramState;

/**
 * 保存程序状态到缓存文件
 * 在运行外部程序前调用
 */
void save_program_state(const char *search_buffer, char **search_results,
                       int result_count, int selected_result,
                       int scroll_offset, int current_window, Options *options);

/**
 * 从缓存文件恢复程序状态
 * 从外部程序返回后调用
 * 返回恢复的状态结构体
 */
ProgramState *restore_program_state();

/**
 * 清理缓存文件
 * 程序退出时调用
 */
void cleanup_cache();

#endif

