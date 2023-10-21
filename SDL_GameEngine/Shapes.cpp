#include "Shapes.h"

void ShapesRendering::DrawFilledSquare(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, new SDL_Rect{ x, y, w, h });
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
}