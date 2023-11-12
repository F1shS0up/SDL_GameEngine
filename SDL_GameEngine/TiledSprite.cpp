#include "TiledSprite.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include "SDL_image.h"

namespace Engine
{
	void TiledSprite_System::Init(Registry* reg, SDL_Renderer* renderer)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->tiledSprites.count(e))
			{
				TiledSprite_Component* c = &reg->tiledSprites[e];
				SDL_Surface* tile = IMG_Load(c->tilePath);

				std::cout << tile->h << " " << tile->w << std::endl;

				if (reg->transforms.count(e))
				{
					int xAmount = std::ceil(c->width / tile->w);
					int yAmount = std::ceil(c->height / tile->h);

					SDL_Surface* finalSurface = SDL_CreateRGBSurface(0, xAmount * tile->w, yAmount * tile->h, 32, 0, 0, 0, 0);

					for (int x = 0; x < xAmount; x++)
					{
						for (int y = 0; y < yAmount; y++)
						{
							SDL_Rect r = { x * tile->w, y * tile->h, tile->w , tile->h };
							SDL_BlitSurface(tile, nullptr, finalSurface, &r);
						}
					}

					c->texture = SDL_CreateTextureFromSurface(renderer, finalSurface);

				}
			}
		}
	}

	void TiledSprite_System::Draw(Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->tiledSprites.count(e))
			{
				if (reg->transforms.count(e))
				{
					SDL_RenderCopy(renderer, reg->tiledSprites[e].texture, nullptr, new SDL_Rect{ (int)(reg->transforms[e].position.x - (reg->tiledSprites[e].moveWithCamera ? cameraRect->x : 0)), (int)(reg->transforms[e].position.y - (reg->tiledSprites[e].moveWithCamera ? cameraRect->y : 0)), reg->tiledSprites[e].width, reg->tiledSprites[e].height });
				}
			}
		}
	}
}
