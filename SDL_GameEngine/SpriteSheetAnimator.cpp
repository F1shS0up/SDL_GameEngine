#include "SpriteSheetAnimator.h"
#include "Entity.h"
#include "Registry.h"

namespace Engine
{
	void SpriteSheetAnimator_Component::Play(const char* animationName, bool fromStart)
	{
		if (fromStart)
		{
			stop = false;
			currentAnimName = animationName;
			currentAnimData = &animationDatas[animationName];
			currentAnimTime = 0;
			currentFrameTime = 0;
			currentAnimFrame = 0;
		}
		else
		{
			if (animationName != currentAnimName)
			{
				stop = false;
				currentAnimName = animationName;
				currentAnimData = &animationDatas[animationName];
				currentAnimTime = 0;
				currentFrameTime = 0;
				currentAnimFrame = 0;
			}
		}
		
	}

	void SpriteSheetAnimator_System::Init(Registry* reg)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->spriteSheetAnimators.count(e) && reg->spriteSheets.count(e))
			{
				SpriteSheetAnimator_Component* anim = &reg->spriteSheetAnimators[e];
				SpriteSheet_Component* spr = &reg->spriteSheets[e];
			}
		}
	}

	void SpriteSheetAnimator_System::Update(Registry* reg, double* deltaTime)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->spriteSheetAnimators.count(e) && reg->spriteSheets.count(e))
			{
				SpriteSheetAnimator_Component* anim = &reg->spriteSheetAnimators[e];
				SpriteSheet_Component* spr = &reg->spriteSheets[e];

				if (anim->stop) continue;

				anim->currentFrameTime += *deltaTime;
				if (anim->currentFrameTime >= anim->currentAnimData->frames[anim->currentAnimFrame].first)
				{
					anim->currentFrameTime = 0;
					anim->currentAnimFrame++;
					if (anim->currentAnimFrame >= anim->currentAnimData->frames.size())
					{
						if (anim->currentAnimData->repeat)
						{
							anim->currentAnimFrame = 0;
						}
						else
						{
							anim->stop = true;
							continue;
						}
					}
					spr->UpdateCurrentSpriteIndex(anim->currentAnimData->frames[anim->currentAnimFrame].second);
				}
			}
		}
	}
}