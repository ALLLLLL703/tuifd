#ifndef OPTIONS_H
#define OPTIONS_H

#include <ncurses.h>
#include <toml.h>


/**
 * 选项相关定义
 * 包含选项结构体和选项管理函数声明
 */

// 排序方式枚举
typedef enum {
    SORT_NAME,      // 按名称排序
    SORT_SIZE,      // 按大小排序
    SORT_MODIFIED   // 按修改时间排序
} SortBy;

// 选项结构体
typedef struct {
    int case_sensitive;     // 是否区分大小写
    int include_hidden;     // 是否包含隐藏文件
    int max_depth;          // 搜索深度限制
    int result_limit;       // 结果数量限制
    char *file_ext;        // 文件类型过滤
    SortBy sort_by;         // 排序方式
    int selected_option;    // 当前选中的选项索引
		char* search_path;
		char* file_type;
} Options;

// 函数声明

// 初始化选项设置
Options init_options(void);

// 显示选项窗口内容
void display_options(WINDOW *win, Options *opts);

// 处理选项窗口输入
void handle_options_input(WINDOW *win, Options *opts, int ch);

// 构建搜索选项字符串
char *build_search_options(Options *opts);

// 显示搜索结果
void display_results(WINDOW *win, char **results, int result_count, int selected, int scroll_offset, Options *opts);

// 打开选中的文件/目录
void open_selected_file(const char *filepath);

// 清理选项内存
void cleanup_options(Options *opts);

#endif
