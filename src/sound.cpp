#include "sound.h"
#include "SDL_assert.h"
#include "SDL_mixer.h"
#include <cstdio>
#include <memory>

#define DISABLE_SOUNDS

#ifdef LOAD_SDL_MIXER_FNS_DYNAMICALLY
// f**** dll won't load so we'll do it manually

#include <windows.h>

typedef Mix_Chunk* (__cdecl* mix_loadwav_ptr)(const char*);
mix_loadwav_ptr Mix_LoadWAV_ptr;

typedef int (__cdecl* mix_playchannel_ptr)(int, Mix_Chunk*, int);
mix_playchannel_ptr Mix_PlayChannel_ptr;

void init_sdl_mixer() {
    HINSTANCE h_get_protocol_id_dll = LoadLibrary("bin\SDL2_mixer.dll");
    SDL_assert_always(h_get_protocol_id_dll && "failed to load SDL2_mixer DLL");

    Mix_LoadWAV_ptr = (mix_loadwav_ptr)GetProcAddress(h_get_protocol_id_dll, "Mix_LoadWAV");
    SDL_assert_always(Mix_LoadWAV_ptr && "failed to get Mix_LoadWAV function");

    Mix_PlayChannel_ptr = (mix_playchannel_ptr)GetProcAddress(h_get_protocol_id_dll, "Mix_PlayChannel");
    SDL_assert_always(Mix_LoadWAV_ptr && "failed to get Mix_LoadWAV function");
}

#define my_Mix_LoadWAV(...) ((*Mix_LoadWAV_ptr)(__VA_ARGS__))
#define my_Mix_PlayChannel(...) ((*Mix_PlayChannel_ptr)(__VA_ARGS__))

#else

void init_sdl_mixer() {}
#define my_Mix_LoadWAV(...) (Mix_LoadWAV(__VA_ARGS__))
#define my_Mix_PlayChannel(...) (Mix_PlayChannel(__VA_ARGS__))

#endif

namespace Tmpl8 {

#define SOUNDS_DIR "assets/sound/"

Mix_Chunk* sounds[1];

bool InitSounds() {
#ifndef DISABLE_SOUNDS
    init_sdl_mixer();

#define LOAD_WAV(ID, NAME) \
    sounds[ID] = my_Mix_LoadWAV(SOUNDS_DIR NAME); \
    SDL_assert_always(sounds[ID] && "Failed to load " SOUNDS_DIR NAME);
    // printf("%s\n", Mix_GetError())
    
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
    my_Mix_PlayChannel(-1, sounds[sound], 0);
#endif
}

}