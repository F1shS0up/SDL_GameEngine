#include "SpriteSheet.h"
#include "Registry.h"
#include "Entity.h"
#include "SDL_image.h"

void Engine::SpriteSheet_System::Init(Registry* reg, SDL_Renderer* renderer)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->spriteSheets.count(e) && reg->transforms.count(e))
		{
			SpriteSheet_Component* c = &reg->spriteSheets[e];
			c->texture = IMG_LoadTexture(renderer, c->path.c_str());
			c->pos = &reg->transforms[e].position;
			c->UpdateCurrentSpriteIndex(0);
		}
	}
}

void Engine::SpriteSheet_System::Render(Registry* reg, SDL_Renderer* renderer)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->spriteSheets.count(e))
		{
			SpriteSheet_Component* c = &reg->spriteSheets[e];
			if (reg->transforms.count(e))
			{
				SDL_RenderCopyEx(renderer, c->texture, &c->srcRect, new SDL_Rect{ (int)(c->pos->x + 0.5), (int)(c->pos->y + 0.5), (int)c->widthAndHeightOfSprite.x, (int)c->widthAndHeightOfSprite.y }, reg->transforms[e].angle, nullptr, c->flip);
			}
		}
	}
}

void Engine::SpriteSheet_Component::UpdateCurrentSpriteIndex(int newIndex)
{
	currentSprite = newIndex;
	srcRect.x = newIndex * widthAndHeightOfSprite.x;
	srcRect.y = 0;
	srcRect.w = widthAndHeightOfSprite.x;
	srcRect.h = widthAndHeightOfSprite.y;
}