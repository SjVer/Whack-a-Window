#include "sound.h"
#include "SDL.h"
#include <cstdio>

namespace Tmpl8 {

#define SOUNDS_DIR "assets/sound/"

struct sounddata {
    SDL_AudioSpec spec;
    Uint32 length, curr_length;
    Uint8* buffer, *curr_buffer;
} sounds[1] = { 0 };

void AudioCallback(void* userdata, Uint8* stream, int len) {
    sounddata* data = (sounddata*)userdata;
    if (data->curr_length <= 0) return;

    len = (len > data->curr_length ? data->curr_length : len);
    SDL_memset(stream, 0, len);
    SDL_MixAudio(stream, data->curr_buffer, len, SDL_MIX_MAXVOLUME);

    data->curr_buffer += len;
    data->curr_length -= len;
}

bool InitSounds() {
#define LOAD_WAV(ID, NAME) do { \
        SDL_assert_always(( \
            SDL_LoadWAV(SOUNDS_DIR ##NAME, \
            &sounds[ID].spec, &sounds[ID].buffer, &sounds[ID].length) \
        ) != NULL); \
        sounds[ID].spec.callback = AudioCallback; \
    } while(0);
    
    LOAD_WAV(SOUND_BOUNCE, "bounce.wav");

#undef LOAD_WAV
    printf("sounds initialized (1 effects).\n");
    return true;
}

// init sounds automatically
bool _discard1 = InitSounds();

int PlaySingleSoundWorker(enum sound sound) {
    sounddata data = sounds[sound];
    data.spec.userdata = &data;
    data.curr_buffer = data.buffer;
    data.curr_length = data.length;

    if (SDL_OpenAudio(&data.spec, NULL) != 0) {
        SDL_ShowSimpleMessageBox(0, "Error", "Audio driver failed to initialize", NULL);
        exit(1);
    }

    SDL_PauseAudio(0);
    while (data.curr_length > 0) SDL_Delay(1);

    SDL_CloseAudio();
    return 0;
}

void PlaySingleSound(enum sound sound) {
    SDL_Thread* thread = SDL_CreateThread(
        (SDL_ThreadFunction)PlaySingleSoundWorker,
        "playsound", (void*)sound
    );
}

}