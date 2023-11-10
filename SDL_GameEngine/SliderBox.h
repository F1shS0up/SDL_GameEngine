#pragma once
#include "SDL.h"
#include "TextBox.h"

struct SliderBox_Component
{
	std::string startName;
	Vector2DInt startNamePos;
	SDL_Color startNameColor;

	const char* fontPath;
	int fontSize;
	SDL_Color color, boxColor, boxColorHovered, boxColorPressed;
	SDL_Rect txtRect, boxRect;
	bool nextDivisionOnXAxis;
	double value;
	double coefficient;
	double min, max;

	bool pressedInside;
	Vector2DInt startPoint;
	double startValue;
	TextBox_Component* txtComp;
};
class SliderBox_System
{
public:
	void Init(class Registry* reg, SDL_Renderer* renderer);
	void Update(class Registry* reg, double* deltaTime, class Game* game);
};