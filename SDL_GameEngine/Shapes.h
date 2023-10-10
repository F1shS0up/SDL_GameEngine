#pragma once
#include "SDL.h"

namespace ShapesRendering
{
    void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius, SDL_Color color);
    void DrawFilledCircle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color color);
}