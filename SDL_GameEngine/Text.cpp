#include "Text.h"
#include <iostream>
#include "SDL_FontCache.h"
#include "Entity.h"
#include "Registry.h"

namespace Engine
{
	void Text_System::Init(Registry* reg, SDL_Renderer* renderer)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->texts.count(e))
			{
				LoadFont(renderer, &reg->texts[e]);
			}
		}
	}
	void Text_System::LoadFont(SDL_Renderer* renderer, Text_Component* comp)
	{
		FC_Font* font = FC_CreateFont();
		FC_LoadFont(font, renderer, comp->fontPath, comp->fontSize, comp->color, TTF_STYLE_NORMAL);
		comp->font = font;
	}

	void Text_System::SetText(Text_Component* component, char* message)
	{
		component->message = message;
	}

	void Text_System::Draw(Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->texts.count(e))
			{
				Text_Component* component = &reg->texts[e];
				FC_Draw(component->font, renderer, component->pos.x, component->pos.y, component->message);
			}
		}
	}
}