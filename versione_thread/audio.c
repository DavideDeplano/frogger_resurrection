#include "header.h"

// Inizializza il sottosistema audio
void initAudio() {
    SDL_setenv("SDL_AUDIODRIVER", "pulseaudio", 1);
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Quit();
        _exit(1);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Quit();
        _exit(1);
    }
    Mix_AllocateChannels(32);
    Mix_Volume(-1, MIX_MAX_VOLUME);
}

// Carica un file audio e restituisce il puntatore al suono
Mix_Chunk *loadSound(const char *file) {
    Mix_Chunk *sound = Mix_LoadWAV(file);
    if (!sound) {
        Mix_CloseAudio();
        SDL_Quit();
        _exit(1);
    }
    return sound;
}

// Riproduce un suono specifico
void playSound(Mix_Chunk *sound, int loop) {
    if (sound) {
        Mix_PlayChannel(-1, sound, loop); // -1 0
    }
}

// Libera la memoria associata a un suono
void freeSound(Mix_Chunk *sound) {
    if (sound) {
        Mix_FreeChunk(sound);
    }
}

// Chiude il sistema audio
void closeAudio() {
    Mix_CloseAudio();
    SDL_Quit();
}
