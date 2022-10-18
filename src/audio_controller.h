#pragma once

#include "framework64/audio.h"

typedef enum {
    AUDIO_CONTROLLER_CHANNEL_PLAYER_WEAPON,
    AUDIO_CONTROLLER_CHANNEL_PLAYER_ACTION,
    AUDIO_CONTROLLER_ENVIRONMENT,
    AUDIO_CONTROLLER_ENVIRONMENT_AMBIENT,
    AUDIO_CONTROLLER_CHANNEL_COUNT
} AudioControllerChannel;

typedef struct {
    fw64Audio* audio;
    int channels[AUDIO_CONTROLLER_CHANNEL_COUNT];
} AudioController;

void audio_controller_init(AudioController* controller, fw64Audio* audio);
void audio_controller_update(AudioController* controller);
int audio_controller_play(AudioController* controller, AudioControllerChannel channel, int sound);
int audio_controller_channel_is_playing(AudioController* controller, AudioControllerChannel channel);