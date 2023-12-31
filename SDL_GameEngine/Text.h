#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Vector2DInt.h"
#include "SDL_FontCache.h"
namespace Engine
{
	struct Text_Component
	{
		const char* message;
		const char* fontPath;
		int fontSize;
		SDL_Color color;
		Vector2DInt pos;

		FC_Font* font;
	};
	class Text_System
	{
	public:
		void LoadFont(SDL_Renderer* renderer, Text_Component* comp);
		void SetText(Text_Component* component, char* message);
		void Render(class Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect);
		void Init(class Registry* reg, SDL_Renderer* renderer);
	};
}
