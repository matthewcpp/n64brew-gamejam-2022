#include "ui.h"
#include "assets/assets.h"
#ifndef PLATFORM_N64
    #include "stdio.h" // sprintf
#endif

void ui_init(UI* ui, fw64Engine* engine, Player* player) {
    ui->engine = engine;
    ui->player = player;
    ui->allocator = fw64_default_allocator();
    ui->font = fw64_font_load(engine->assets, FW64_ASSET_font_level_select, ui->allocator);
}

void ui_uninit(UI* ui) {
    fw64_font_delete(ui->engine->assets, ui->font, ui->allocator);
}

static void ui_draw_player_weapon_crosshair(UI* ui) {
    fw64Renderer* renderer = ui->engine->renderer;
    fw64Texture* crosshair = ui->player->weapon.crosshair;

    IVec2 screen_size, crosshair_pos;
    fw64_renderer_get_screen_size(renderer, &screen_size);
    crosshair_pos.x = screen_size.x / 2 - fw64_texture_width(crosshair) / 2;
    crosshair_pos.y = screen_size.y / 2 - fw64_texture_height(crosshair) / 2;

    fw64_renderer_draw_sprite(renderer, crosshair, crosshair_pos.x, crosshair_pos.y);
}

static void ui_draw_fps(UI* ui) { 
#ifndef NDEBUG
    fw64Renderer* renderer = ui->engine->renderer;
    static char debug_msg[80]= {0};
    #define FRAMETIME_COUNT 30
    #if defined(PLATFORM_N64)
           
        static OSTime frameTimes[FRAMETIME_COUNT];
        static u8 curFrameTimeIndex = 0;
        static u32 sCPU = 0;
        static f32 gFPS = 0;
        u32 tmem = IO_READ(DPC_TMEM_REG);
        u32 cmd =  MAX(IO_READ(DPC_BUFBUSY_REG), tmem);
        u32 pipe = MAX(IO_READ(DPC_PIPEBUSY_REG), cmd);
        OSTime newTime = osGetTime();
        OSTime oldTime = frameTimes[curFrameTimeIndex];
        frameTimes[curFrameTimeIndex] = newTime;
        curFrameTimeIndex++;
        if (curFrameTimeIndex >= FRAMETIME_COUNT) {
            curFrameTimeIndex = 0;
        }
        gFPS = ((f32)FRAMETIME_COUNT * 1000000.0f) / (s32)OS_CYCLES_TO_USEC(newTime - oldTime);    
        IO_WRITE(DPC_STATUS_REG, (DPC_CLR_CLOCK_CTR | DPC_CLR_CMD_CTR | DPC_CLR_PIPE_CTR | DPC_CLR_TMEM_CTR));
        sprintf(debug_msg,"fps: %2.2f    cpu %d    rdp %d", gFPS, (u32) OS_CYCLES_TO_USEC(sCPU), (u32) (pipe * 10) / 625);
        fw64_renderer_draw_text(renderer, ui->font, 8, 8, debug_msg);

    #else
        static uint64_t frameTimes[FRAMETIME_COUNT];
        static uint8_t curFrameTimeIndex = 0;
        if(ui->engine->time->time_delta < 0.0000001)
            return;
        uint64_t newTime = ui->engine->time->time_delta;
        uint64_t oldTime = frameTimes[curFrameTimeIndex];
        frameTimes[curFrameTimeIndex] = newTime;
        curFrameTimeIndex++;
        if (curFrameTimeIndex >= FRAMETIME_COUNT) {
            curFrameTimeIndex = 0;
        }
        float fps = 1.0f / (ui->engine->time->time_delta);
        sprintf(debug_msg,"fps: %2.2f", fps);
        fw64_renderer_draw_text(renderer, ui->font, 8, 8, debug_msg);
    #endif // defined(PLATFORM_N64)  
#endif // ifndef NDEBUG    
}

void ui_draw(UI* ui) {
    if (ui->player->weapon.type != WEAPON_TYPE_NONE) {
        ui_draw_player_weapon_crosshair(ui);
    }
    ui_draw_fps(ui);
}