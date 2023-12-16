#include "Shape.h"
#include "Entity.h"
#include "Shapes.h"
#include "Registry.h"
#include "SDL2_gfxPrimitives.h"
namespace Engine
{
	void Shape_System::Init(Registry* reg)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->transforms.count(e) && reg->filledCircles.count(e))
			{
				reg->filledCircles[e].position = &reg->transforms[e].position;
			}
			if (reg->transforms.count(e) && reg->filledRectangles.count(e))
			{
				reg->filledRectangles[e].position = &reg->transforms[e].position;
			}
		}
	}

	void Shape_System::Render(Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->filledCircles.count(e))
			{
				filledCircleRGBA(renderer, (*reg->filledCircles[e].position).x, (*reg->filledCircles[e].position).y, reg->filledCircles[e].radius, reg->filledCircles[e].color.r, reg->filledCircles[e].color.g, reg->filledCircles[e].color.b, reg->filledCircles[e].color.a);
			}
			if (reg->filledRectangles.count(e))
			{
				ShapesRendering::DrawFilledSquare(renderer, reg->filledRectangles[e].position->x, reg->filledRectangles[e].position->y, reg->filledRectangles[e].width, reg->filledRectangles[e].height, reg->filledRectangles[e].color);
			}
		}
	}
}