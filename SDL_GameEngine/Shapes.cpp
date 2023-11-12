#include "Shapes.h"

namespace Engine
{
	void ShapesRendering::DrawFilledSquare(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color)
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, new SDL_Rect{ x, y, w, h });
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
	}
	void ShapesRendering::DrawTiled(SDL_Surface* bmp, SDL_Surface* tile, int offset_x, int offset_y, int screenW, int screenH)
	{
		// Draw the background tiles
		int x = offset_x, y = offset_y;
		int w = tile->w;
		int h = tile->h;
		while (1)
		{
			// Draw a tile to the current position
			SDL_Rect r;
			r.x = x;
			r.y = y;
			r.w = w;
			r.h = h;

			SDL_BlitSurface(tile, NULL, bmp, &r);

			// Advance
			x += w;
			if (x >= screenW)
			{
				x = offset_x;
				y += h;
				if (y >= screenH)
					break;
			}
		}
	}
}