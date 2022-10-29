#pragma once

#include "framework64/engine.h"

typedef enum {
    DIALOGUE_WINDOW_STATUS_INACTIVE,
    DIALOGUE_WINDOW_STATUS_WRITING,
    DIALOGUE_WINDOW_STATUS_WAITING,
    DIALOGUE_WINDOW_STATUS_DONE
} DialogueWindowStatus;

#define SPEECH_DIALOG_CHARACTER_WRITE_TIME (1.0f / 12.0f)
#define DIALOGUE_WINDOW_LINE_COUNT 2

typedef struct {
    char* text;
    int count;
}DialogueWindowLine;

typedef void(*DialogueWindowDoneCallback)(void*);

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64Texture* background_texture;
    fw64Texture* next_indicator;
    DialogueWindowStatus status;
    uint32_t data_size;
    char* dialogue_data;

    DialogueWindowLine lines[DIALOGUE_WINDOW_LINE_COUNT];
    int current_line_index;

    float current_character_time;
    float character_write_time;
    int current_line_size;

    int current_data_index;

    IVec2 position, size;
    DialogueWindowDoneCallback done_callback;
    void* callbark_arg;
} DialogueWindow;

#ifdef __cplusplus
extern "C" {
#endif

void dialogue_window_init(DialogueWindow* window, fw64Engine* engine, int font_assset, int dialogue_asset, IVec2* position, IVec2* size, fw64Allocator* allocator);
void dialogue_window_uninit(DialogueWindow* window, fw64Allocator* allocator);
void dialogue_window_update(DialogueWindow* window);
void dialogue_window_draw(DialogueWindow* window);

void dialogue_window_start(DialogueWindow* window, DialogueWindowDoneCallback callback, void* arg);

#ifdef __cplusplus
}
#endif