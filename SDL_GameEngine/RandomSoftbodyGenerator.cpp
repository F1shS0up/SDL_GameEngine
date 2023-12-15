#include "RandomSoftbodyGenerator.h"
#include "Entity.h"
#include "Registry.h"
#include <random>
#include "Game.h"

namespace Engine
{
	void RandomSoftbodyGenerator_System::Init(Registry* reg, Game* game)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->randomSoftbodyGenerators.count(e) && reg->softbodies.count(e))
			{
				RandomSoftbodyGenerator_Component* c = &reg->randomSoftbodyGenerators[e];
				Softbody_Component* cS = &reg->softbodies[e];
				int amount = c->amountOfPointsDividedByTwo;
				std::vector<Vector2D> points;

				std::random_device rd; // obtain a random number from hardware
				std::mt19937 gen(rd()); // seed the generator
				std::uniform_int_distribution<> rangeX(c->minX, c->maxX);
				std::uniform_int_distribution<> rangeY(c->minY, c->maxY);

				Vector2D lastPointPos = c->pointNO1Pos;
				points.push_back(lastPointPos);
				for (int i = 1; i < amount; i++)
				{
					Vector2D pos = lastPointPos + Vector2D(rangeX(gen), rangeY(gen));
					points.push_back(pos);
					lastPointPos = pos;
				}
				for (int i = amount - 1; i >= 0; i--)
				{
					if (c->tp == HorizontalLinePolygon)
					{
						Vector2D pos = points[i] + c->offsetOfOtherLine;
						points.push_back(pos);
					}
				}
				cS->massPointsN = points.size();
				for (int i = 0; i < points.size();i++)
				{
					cS->massPoints.push_back(MassPoint{ points[i], 1, 1, c->arePointsStatic });
				}
			}
		}
	}
}