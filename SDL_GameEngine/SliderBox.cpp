#include "SliderBox.h"
#include "Registry.h"
#include "Entity.h"
#include "InputManager.h"

const float coefficient = 1;

void SliderBox_System::Init(Registry* reg, SDL_Renderer* renderer)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->sliderBoxes.count(e))
		{
			SliderBox_Component* c = &reg->sliderBoxes[e];
			reg->textBoxes[e] = TextBox_Component{ "", c->fontPath, c->fontSize, c->color, c->boxColor, c->txtRect, c->boxRect, false};
			c->txtComp = &reg->textBoxes[e];
			c->txtComp->message = std::to_string(c->value);

			reg->texts[e] = Text_Component{ c->startName.c_str(), c->fontPath, c->fontSize, c->startNameColor, c->startNamePos};
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
			if (InputManager::Instance()->MouseButtonPressed(InputManager::left))
			{
				if (Registry::Instance()->textBoxSystem.IsVectorInBox(c->txtComp, InputManager::Instance()->MousePos()) == SDL_TRUE)
				{
					c->pressedInside = true;
					c->startPoint = InputManager::Instance()->MousePos();
					c->startValue = c->value;
					
				}
			}
			else if (InputManager::Instance()->MouseButtonReleased(InputManager::left))
			{
				c->pressedInside = false;
			}

			if (c->pressedInside)
			{
				//Difference between the starting position when clicked on element and current mouse position
				float difference = c->onXAxis ? InputManager::Instance()->MousePos().x - c->startPoint.x : InputManager::Instance()->MousePos().y - c->startPoint.y;

				c->value = c->startValue + difference * coefficient;
				c->txtComp->message = std::to_string(c->value);
			}
		}
	}
}
