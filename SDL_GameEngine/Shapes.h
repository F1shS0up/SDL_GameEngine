#pragma once
#include "SDL.h"

namespace ShapesRendering
{
	void DrawFilledSquare(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color);
	void DrawTiled(SDL_Surface* bmp, SDL_Surface* tile, int offset_x, int offset_y, int recW, int recH);
}