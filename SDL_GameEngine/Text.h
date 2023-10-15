#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Vector2DInt.h"

struct Text_Component
{
	const char* message;
	const char* fontPath;
	int fontSize;
	SDL_Color color;
	Vector2DInt pos;

	class FC_Font* font;
};
class Text_System
{
public:
	void LoadFont(SDL_Renderer* renderer, Text_Component* comp);
	void SetText(Text_Component* component, char* message);
	void Draw(class Registry* reg, SDL_Renderer* renderer);
	void Init(class Registry* reg, SDL_Renderer* renderer);
};
