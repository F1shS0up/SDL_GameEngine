#pragma once
#include "Vector2D.h"
#include "SDL.h"

struct FilledCircle_Component
{
	double radius;
	SDL_Color color;

	Vector2D* position;
};
struct FilledRectangle_Component
{
	double width, height;
	SDL_Color color;

	Vector2D* position;
};

class Shape_System
{
public:
	void Init(class Registry* reg);
	void Draw(class Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect);
};
