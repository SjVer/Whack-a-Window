#include "sound.h"
#include "SDL.h"
#include <cstdio>

namespace Tmpl8 {

#define SOUNDS_DIR "assets/converted_font/out/"

struct {
    SDL_AudioSpec spec;
    Uint32 length;
    Uint8* buffer;
} sounds[1] = { 0 };

bool InitSounds() {
#define LOAD_WAV(ID, NAME) \
    SDL_assert_always(( \
        SDL_LoadWAV(SOUNDS_DIR ##NAME, \
        &sounds[ID].spec, &sounds[ID].buffer, &sounds[ID].length) \
    ) == NULL);
    
    LOAD_WAV(SOUND_BOUNCE, "bounce.wav");

#undef LOAD_WAV
    printf("sounds initialized (1 effects).\n");
    return true;
}

// init sounds automatically
bool _discard1 = InitSounds();

void playSound(enum sound sound) {
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, NULL, NULL, 0);

    if (deviceId)
    {
        int success = SDL_QueueAudio(deviceId, sounds[sound].buffer, sounds[sound].length);

        SDL_PauseAudioDevice(deviceId, 0);

        SDL_Delay(3000);
        SDL_CloseAudioDevice(deviceId);
    }
    else
        SDL_ShowSimpleMessageBox(0, "Error", "Audio driver failed to initialize", NULL);

}

}