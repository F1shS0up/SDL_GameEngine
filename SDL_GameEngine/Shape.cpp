#include "Shape.h"
#include "Entity.h"
#include "Shapes.h"
#include "Registry.h"

void Shape_System::Init(Registry* reg)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->transforms.count(e) && reg->circlesFilled.count(e))
		{
			reg->circlesFilled[e].position = &reg->transforms[e].position;
		}
	}
}

void Shape_System::Draw(Registry* reg, SDL_Renderer* renderer)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->circlesFilled.count(e))
		{
			ShapesRendering::DrawFilledCircle(renderer, (*reg->circlesFilled[e].position).x, (*reg->circlesFilled[e].position).y, reg->circlesFilled[e].radius, reg->circlesFilled[e].color);
		}
	}
}
