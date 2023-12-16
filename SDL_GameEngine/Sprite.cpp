#include "Sprite.h"
#include "Entity.h"
#include "Registry.h"
#include "SDL_image.h"

namespace Engine
{
	void Sprite_System::Init(Registry* reg, SDL_Renderer* renderer)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->sprites.count(e))
			{
				Sprite_Component* c = &reg->sprites[e];
				c->texture = IMG_LoadTexture(renderer, c->path.c_str());
				if (c->dstRect->w == 0)
				{
					SDL_QueryTexture(c->texture, nullptr, nullptr, &c->dstRect->w, &c->dstRect->h);
				}
			}
		}
	}

	void Sprite_System::Render(Registry* reg, SDL_Renderer* renderer)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->sprites.count(e))
			{
				Sprite_Component* c = &reg->sprites[e];
				if (reg->transforms.count(e))
				{
					SDL_RenderCopyEx(renderer, c->texture, nullptr, new SDL_Rect{ (int)reg->transforms[e].position.x, (int)reg->transforms[e].position.y, c->dstRect->w, c->dstRect->h }, reg->transforms[e].angle, nullptr, c->flip);
				}
			}
		}
	}
}