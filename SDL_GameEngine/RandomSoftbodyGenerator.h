#pragma once
#include "SDL.h"
#include "Vector2D.h"
#include "Softbody.h"

namespace Engine
{
	enum RandomSoftbodyGeneration_Type
	{
		HorizontalLinePolygon,
		VerticalLinePolygon,
	};

	struct RandomSoftbodyGenerator_Component
	{
		RandomSoftbodyGeneration_Type tp;
		int amountOfPointsDividedByTwo;
		Vector2D pointNO1Pos;

		Sint16 minX, maxX;
		Sint16 minY, maxY;

		bool arePointsStatic = false;

		Vector2D offsetOfOtherLine;
	};

	class RandomSoftbodyGenerator_System
	{
	public:
		void Init(class Registry* reg, Game* game);
	};
}