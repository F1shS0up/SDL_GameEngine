#pragma once
#include "SDL.h"

namespace Engine
{
	struct TiledSprite_Component
	{
		const char* tilePath;
		int width, height;
		bool moveWithCamera;

		SDL_Texture* texture;
	};

	class TiledSprite_System
	{
	public:
		void Init(class Registry* reg, SDL_Renderer* renderer);
		void Draw(class Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect);
	};
}