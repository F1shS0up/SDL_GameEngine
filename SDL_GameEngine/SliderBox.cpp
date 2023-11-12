#include "SliderBox.h"
#include "Registry.h"
#include "Entity.h"
#include "InputManager.h"
namespace Engine
{
	void SliderBox_System::Init(Registry* reg, SDL_Renderer* renderer)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->sliderBoxes.count(e))
			{
				SliderBox_Component* c = &reg->sliderBoxes[e];
				reg->textBoxes[e] = TextBox_Component{ "", c->fontPath, c->fontSize, c->color, c->boxColor, c->txtRect, c->boxRect, false };
				c->txtComp = &reg->textBoxes[e];
				c->txtComp->message = std::to_string(c->value);

				reg->texts[e] = Text_Component{ c->startName.c_str(), c->fontPath, c->fontSize, c->startNameColor, c->startNamePos };
			}
		}
	}

	void SliderBox_System::Update(Registry* reg, double* deltaTime, Game* game)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->sliderBoxes.count(e))
			{
				SliderBox_Component* c = &reg->sliderBoxes[e];
				if (Registry::Instance()->textBoxSystem.IsVectorInBox(c->txtComp, InputManager::Instance()->MousePos()) == SDL_TRUE)
				{
					if (!c->pressedInside)
					{
						c->txtComp->boxColor = c->boxColorHovered;
						SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
						SDL_SetCursor(cursor);
					}
					if (InputManager::Instance()->MouseButtonPressed(InputManager::left))
					{

						c->pressedInside = true;
						c->startPoint = InputManager::Instance()->MousePos();
						c->startValue = c->value;
						c->txtComp->boxColor = c->boxColorPressed;
						SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
						SDL_SetCursor(cursor);

					}
				}

				if (InputManager::Instance()->MouseButtonReleased(InputManager::left))
				{
					c->pressedInside = false;
					c->txtComp->boxColor = c->boxColor;
					SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
					SDL_SetCursor(cursor);
				}
				else if (!c->pressedInside && Registry::Instance()->textBoxSystem.IsVectorInBox(c->txtComp, InputManager::Instance()->MousePos()) != SDL_TRUE)
				{
					c->txtComp->boxColor = c->boxColor;
				}


				if (c->pressedInside)
				{
					//Difference between the starting position when clicked on element and current mouse position
					double difference = c->nextDivisionOnXAxis ? InputManager::Instance()->MousePos().x - c->startPoint.x : InputManager::Instance()->MousePos().y - c->startPoint.y;

					c->value = std::clamp <double>(c->startValue + difference * c->coefficient, c->min, c->max);
					c->txtComp->message = std::to_string(c->value);
				}


			}
		}
	}
}
