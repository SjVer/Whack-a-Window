#include "sound.h"
#include "SDL_assert.h"
#include "SDL_mixer.h"
#include <cstdio>
#include <memory>

#define DISABLE_SOUNDS

namespace Tmpl8 {

#define SOUNDS_DIR "assets/sound/"

Mix_Chunk* sounds[1];

bool InitSounds() {
#ifndef DISABLE_SOUNDS
#define LOAD_WAV(ID, NAME) \
    sounds[ID] = Mix_LoadWAV(SOUNDS_DIR NAME); \
    SDL_assert_always(sounds[ID] && "Failed to load " SOUNDS_DIR NAME);
    
    LOAD_WAV(SOUND_BOUNCE, "bounce.wav");

#undef LOAD_WAV
    printf("sounds initialized (1 effects).\n");
#endif
    return true;
}

// init sounds automatically
bool _discard1 = InitSounds();

void PlaySingleSound(enum sound sound) {
#ifndef DISABLE_SOUNDS
    Mix_PlayChannel(-1, sounds[sound], 0);
#endif
}

}