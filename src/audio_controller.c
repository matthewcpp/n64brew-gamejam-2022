#include "audio_controller.h"

void audio_controller_init(AudioController* controller, fw64Audio* audio) {
    controller->audio = audio;

    for (int i = 0; i < AUDIO_CONTROLLER_CHANNEL_COUNT; i++) {
        controller->channels[i] = -1;
    }
}

void audio_controller_update(AudioController* controller) {
    for (int i = 0; i < AUDIO_CONTROLLER_CHANNEL_COUNT; i++) {
        if (controller->channels[i] == -1)
            continue;

        int status = fw64_audio_get_sound_status(controller->audio, controller->channels[i]);
        if (status == FW64_AUDIO_STOPPED)
            controller->channels[i] = -1;
    }
}

int audio_controller_play(AudioController* controller, AudioControllerChannel channel, int sound) {
    if (controller->channels[channel] >= 0)
        fw64_audio_stop_sound(controller->audio, controller->channels[channel]);

    int handle = fw64_audio_play_sound(controller->audio, sound);
    controller->channels[channel] = handle;

    return handle;
}