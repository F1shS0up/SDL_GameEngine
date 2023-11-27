#pragma once
#include <string>
#include "SDL.h"
namespace Engine
{
	struct Sprite_Component
	{
		std::string path;

		//Set width to zero in order to set dst rect as texture size
		SDL_Rect* dstRect;

		SDL_Texture* texture;
	};

	class Sprite_System
	{
	public:
		void Init(class Registry* reg, SDL_Renderer* renderer);
		void Draw(class Registry* reg, SDL_Renderer* renderer);
	};
}
