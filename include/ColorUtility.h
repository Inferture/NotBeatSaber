#ifndef COLORUTILITY_H
#define COLORUTILITY_H

#include <SDL2/SDL.h>

SDL_Color RandomWallColor();
SDL_Color GetNextColor(SDL_Color current, SDL_Color target, uint8_t speed);
SDL_Color RGBtoGBR(SDL_Color origin);
SDL_Color MultiplyColor(SDL_Color c, float f);
SDL_Color Negative(SDL_Color c);


#endif // COLORUTILITY_H
