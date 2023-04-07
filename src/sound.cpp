#include "sound.h"
#include "SDL_assert.h"
#include "SDL_mixer.h"
#include <cstdio>
#include <memory>

namespace Tmpl8 {

#define SOUNDS_DIR "assets/sound/"

Mix_Chunk* sounds[1];

bool InitSounds() {
#define LOAD_WAV(ID, NAME) \
    sounds[ID] = Mix_LoadWAV(SOUNDS_DIR ##NAME); \
    SDL_assert_always(sounds[ID]); \
    
    LOAD_WAV(SOUND_BOUNCE, "bounce.wav");

#undef LOAD_WAV
    printf("sounds initialized (1 effects).\n");
    return true;
}

// init sounds automatically
bool _discard1 = InitSounds();

void PlaySingleSound(enum sound sound) {
    Mix_PlayChannel(-1, sounds[sound], 0);
}

}