#include "Button.h"
#include "Registry.h"
#include "Entity.h"
#include "InputManager.h"
namespace Engine
{
	void Button_System::Init(Registry* reg, SDL_Renderer* renderer)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->buttons.count(e))
			{
				Button_Component* c = &reg->buttons[e];
				reg->textBoxes[e] = TextBox_Component{ c->message, c->fontPath, c->fontSize, c->color, c->boxColor, c->txtRect, c->boxRect, false };
				c->txtComp = &reg->textBoxes[e];
			}
		}
	}

	void Button_System::Update(Registry* reg, double* deltaTime, Game* game)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->buttons.count(e))
			{
				Button_Component* c = &reg->buttons[e];
				if (Registry::Instance()->textBoxSystem.IsVectorInBox(c->txtComp, InputManager::Instance()->MousePos()) == SDL_TRUE)
				{
					if (!c->pressedInside)
					{
						c->txtComp->boxColor = c->boxColorHovered;
						//SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
						//SDL_SetCursor(cursor);
					}
					if (InputManager::Instance()->MouseButtonPressed(InputManager::left))
					{
						c->pressedInside = true;
						c->txtComp->boxColor = c->boxColorPressed;
						c->func(game);
						//SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
						//SDL_SetCursor(cursor);
					}
				}
				if (InputManager::Instance()->MouseButtonReleased(InputManager::left))
				{
					c->pressedInside = false;
					c->txtComp->boxColor = c->boxColor;
					//SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
					//SDL_SetCursor(cursor);
				}
				else if (!c->pressedInside && Registry::Instance()->textBoxSystem.IsVectorInBox(c->txtComp, InputManager::Instance()->MousePos()) != SDL_TRUE)
				{
					c->txtComp->boxColor = c->boxColor;
				}
			}
		}
	}
}