#include "TextBox.h"
#include "Registry.h"
#include "Entity.h"
#include "SDL_FontCache.h"
#include "Shapes.h"
#include "Game.h"

void TextBox_System::Init(Registry* reg, SDL_Renderer* renderer)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->textBoxes.count(e))
		{
			LoadFont(renderer, &reg->textBoxes[e]);
		}
	}
}

void TextBox_System::Draw(Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->textBoxes.count(e))
		{
			TextBox_Component* component = &reg->textBoxes[e];
			ShapesRendering::DrawFilledSquare(renderer, component->boxRect.x, component->boxRect.y, component->boxRect.w, component->boxRect.h, component->boxColor);
			FC_DrawBox(component->font, renderer, component->txtRect, component->message.c_str());
		}
	}
}

void TextBox_System::Update(Registry* reg, double* deltaTime, Game* game)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->textBoxes.count(e))
		{
			TextBox_Component* c = &reg->textBoxes[e];
			if (!c->inputField) continue;

			if (InputManager::Instance()->MouseButtonDown(InputManager::left))
			{
				if (IsVectorInBox(c, InputManager::Instance()->MousePos()) == SDL_TRUE)
				{
					game->SetTextInputString(&c->message);
					SDL_StartTextInput();
				}
			}
		}
	}
}

void TextBox_System::LoadFont(SDL_Renderer* renderer, TextBox_Component* comp)
{
	FC_Font* font = FC_CreateFont();
	FC_LoadFont(font, renderer, comp->fontPath, comp->fontSize, comp->color, TTF_STYLE_NORMAL);
	comp->font = font;
}

void TextBox_System::SetText(TextBox_Component* component, char* message)
{
	component->message = message;
}


bool TextBox_System::IsVectorInBox(TextBox_Component* comp, Vector2D point)
{
	SDL_Point p = SDL_Point{ (int)point.x, (int)point.y };
	return SDL_PointInRect(&p, &comp->boxRect);
}

bool TextBox_System::IsVectorInBox(TextBox_Component* comp, Vector2DInt point)
{
	SDL_Point p = SDL_Point{ point.x, point.y };
	return SDL_PointInRect(&p, &comp->boxRect);
}


