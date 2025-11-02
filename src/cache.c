#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

/**
 * 确保缓存目录存在
 */
static void ensure_cache_dir() {
    char cache_dir[256];
    snprintf(cache_dir, sizeof(cache_dir), "%s/.cache/tuifd", getenv("HOME"));

    struct stat st = {0};
    if (stat(cache_dir, &st) == -1) {
        mkdir(cache_dir, 0700);
    }
}

/**
 * 保存程序状态到缓存文件
 */
void save_program_state(const char *search_buffer, char **search_results,
                       int result_count, int selected_result,
                       int scroll_offset, int current_window, Options *options) {
    ensure_cache_dir();

    char cache_file[256];
    snprintf(cache_file, sizeof(cache_file), "%s/.cache/tuifd/state.cache", getenv("HOME"));

    FILE *fp = fopen(cache_file, "wb");
    if (!fp) {
        return;
    }

    // 写入基本状态信息
    fwrite(&result_count, sizeof(int), 1, fp);
    fwrite(&selected_result, sizeof(int), 1, fp);
    fwrite(&scroll_offset, sizeof(int), 1, fp);
    fwrite(&current_window, sizeof(int), 1, fp);

    // 写入搜索缓冲区
    size_t search_len = strlen(search_buffer);
    fwrite(&search_len, sizeof(size_t), 1, fp);
    fwrite(search_buffer, sizeof(char), search_len, fp);

    // 写入选项设置
    fwrite(options, sizeof(Options), 1, fp);

    // 写入文件类型字符串长度（如果有）
    size_t file_type_len = options->file_type ? strlen(options->file_type) : 0;
    fwrite(&file_type_len, sizeof(size_t), 1, fp);
    if (file_type_len > 0) {
        fwrite(options->file_type, sizeof(char), file_type_len, fp);
    }

    // 写入搜索结果
    for (int i = 0; i < result_count; i++) {
        size_t len = strlen(search_results[i]);
        fwrite(&len, sizeof(size_t), 1, fp);
        fwrite(search_results[i], sizeof(char), len, fp);
    }

    fclose(fp);
}

/**
 * 从缓存文件恢复程序状态
 */
ProgramState *restore_program_state() {
    char cache_file[256];
    snprintf(cache_file, sizeof(cache_file), "%s/.cache/tuifd/state.cache", getenv("HOME"));

    FILE *fp = fopen(cache_file, "rb");
    if (!fp) {
        return NULL;
    }

    ProgramState *state = malloc(sizeof(ProgramState));
    if (!state) {
        fclose(fp);
        return NULL;
    }

    // 读取基本状态信息
    fread(&state->result_count, sizeof(int), 1, fp);
    fread(&state->selected_result, sizeof(int), 1, fp);
    fread(&state->scroll_offset, sizeof(int), 1, fp);
    fread(&state->current_window, sizeof(int), 1, fp);

    // 读取搜索缓冲区
    size_t search_len;
    fread(&search_len, sizeof(size_t), 1, fp);
    fread(state->search_buffer, sizeof(char), search_len, fp);
    state->search_buffer[search_len] = '\0';

    // 读取选项设置
    fread(&state->options, sizeof(Options), 1, fp);

    // 读取文件类型字符串
    size_t file_type_len;
    fread(&file_type_len, sizeof(size_t), 1, fp);
    if (file_type_len > 0) {
        state->options.file_type = malloc(file_type_len + 1);
        fread(state->options.file_type, sizeof(char), file_type_len, fp);
        state->options.file_type[file_type_len] = '\0';
    } else {
        state->options.file_type = NULL;
    }

    // 读取搜索结果
    if (state->result_count > 0) {
        state->search_results = malloc(state->result_count * sizeof(char *));
        for (int i = 0; i < state->result_count; i++) {
            size_t len;
            fread(&len, sizeof(size_t), 1, fp);
            state->search_results[i] = malloc(len + 1);
            fread(state->search_results[i], sizeof(char), len, fp);
            state->search_results[i][len] = '\0';
        }
    } else {
        state->search_results = NULL;
    }

    fclose(fp);
    return state;
}

/**
 * 清理缓存文件
 */
void cleanup_cache() {
    char cache_file[256];
    snprintf(cache_file, sizeof(cache_file), "%s/.cache/tuifd/state.cache", getenv("HOME"));

    remove(cache_file);
}

