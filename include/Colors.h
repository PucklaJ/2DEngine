#ifndef COLORS_H
#define COLORS_H
#include <SDL2/SDL.h>

namespace SDL
{
    namespace Colors
    {
        const SDL_Color RED = {255,0,0,255};
        const SDL_Color GREEN = {0,255,0,255};
        const SDL_Color BLUE = {0,0,255,255};
        const SDL_Color WHITE = {255,255,255,255};
        const SDL_Color BLACK = {0,0,0,255};
        const SDL_Color GRAY = {128,128,128,255};
        const SDL_Color YELLOW = {255,255,0,255};
    }
}

#endif