#pragma once
#include "SDL.h"
#include <string>
#include "Vector2D.h"
#include "TextBox.h"

namespace Engine
{
	struct Button_Component
	{
		std::string message;
		const char* fontPath;
		int fontSize;
		SDL_Color color, boxColor, boxColorHovered, boxColorPressed;
		SDL_Rect txtRect, boxRect;
		void(*func)(Game*);

		bool pressedInside;
		TextBox_Component* txtComp;
	};
	class Button_System
	{
	public:
		void Init(class Registry* reg, SDL_Renderer* renderer);
		void Update(class Registry* reg, double* deltaTime, class Game* game);
	};
}