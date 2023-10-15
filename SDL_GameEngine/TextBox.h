#pragma once
#include "SDL.h"
#include "Vector2D.h"
#include "Vector2DInt.h"
#include <string>

struct TextBox_Component
{
	std::string message;
	const char* fontPath;
	int fontSize;
	SDL_Color color, boxColor;
	SDL_Rect txtRect, boxRect;
	bool inputField;

	class FC_Font* font;
};
class TextBox_System
{
public:
	void Init(class Registry* reg, SDL_Renderer* renderer);
	void Draw(class Registry* reg, SDL_Renderer* renderer);
	void Update(class Registry* reg, double* deltaTime, class Game* game);
	void LoadFont(SDL_Renderer* renderer, TextBox_Component* comp);
	void SetText(TextBox_Component* component, char* message);
	bool IsVectorInBox(TextBox_Component* comp, Vector2D point);
	bool IsVectorInBox(TextBox_Component* comp, Vector2DInt point);
};