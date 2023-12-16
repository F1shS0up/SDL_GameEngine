#pragma once
#include "SDL.h"
#include <string>
#include "Vector2D.h"

namespace Engine
{
	struct SpriteSheet_Component
	{
		std::string path;

		Vector2D widthAndHeightOfSprite;
		int numberOfSprites;

		SDL_RendererFlip flip;
		Vector2D* pos;
		SDL_Rect srcRect;
		int currentSprite;
		SDL_Texture* texture;

		void UpdateCurrentSpriteIndex(int newIndex);
	};

	class SpriteSheet_System
	{
	public:
		void Init(class Registry* reg, SDL_Renderer* renderer);
		void Render(class Registry* reg, SDL_Renderer* renderer);
	};
}
