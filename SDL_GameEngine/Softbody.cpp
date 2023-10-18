#include "Softbody.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"

void Softbody_System::Init(Registry* reg)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				reg->softbodies[e].massPoints[p].velocity = Vector2D(0, 0);
			}
		}
	}
}

void Softbody_System::Update(Registry* reg, double* deltaTime)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				reg->softbodies[e].massPoints[p].force = Vector2D(0, 0);
				reg->softbodies[e].massPoints[p].force.y = *reg->softbodies[e].gravity;

				reg->softbodies[e].massPoints[p].velocity += reg->softbodies[e].massPoints[p].force * *deltaTime;

				SDL_Point A = { reg->softbodies[e].massPoints[p].position.x, reg->softbodies[e].massPoints[p].position.y };
				int intersectionCount = 0;
				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					
					if (reg->AABBColliders.count(e2))
					{
						SDL_Rect r = { reg->AABBColliders[e2].position->x, reg->AABBColliders[e2].position->y, reg->AABBColliders[e2].width, reg->AABBColliders[e2].height };
						if (SDL_PointInRect(&A, &r))
						{
							Vector2D min = *reg->AABBColliders[e2].position;
							Vector2D max = *reg->AABBColliders[e2].position + Vector2D(reg->AABBColliders[e2].width, reg->AABBColliders[e2].height);

							float yTop = std::abs(A.y - min.y);
							float yDown = std::abs(A.y - max.y);
							float xRight = std::abs(A.x - max.y);
							float xLeft = std::abs(A.x - min.y);

							std::cout << yTop << " " << yDown << " " << xRight << " " << xLeft << std::endl;

							


							if (xRight < xLeft)
							{
								if (yTop < yDown)
								{
									if (xRight < yTop)
										//Right
										reg->softbodies[e].massPoints[p].position.x = max.x;
									else
										//Top
										reg->softbodies[e].massPoints[p].position.y = min.y;
								}
								else
								{
									if (xRight < yDown)
										//Right
										reg->softbodies[e].massPoints[p].position.x = max.x;
									else
										//Top
										reg->softbodies[e].massPoints[p].position.y = max.y;
								}
							}
							else
							{
								if (yTop < yDown)
								{
									if (xLeft < yTop)
										//Right
										reg->softbodies[e].massPoints[p].position.x = min.x;
									else
										//Top
										reg->softbodies[e].massPoints[p].position.y = min.y;
								}
								else
								{
									if (xLeft < yDown)
										//Right
										reg->softbodies[e].massPoints[p].position.x = min.x;
									else
										//Top
										reg->softbodies[e].massPoints[p].position.y = max.y;
								}
							}
							return;
						}
					}
				}

				

				reg->softbodies[e].massPoints[p].position += reg->softbodies[e].massPoints[p].velocity * *deltaTime;
			}
		}
	}
}

void Softbody_System::Draw(Registry* reg, SDL_Renderer* renderer)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				ShapesRendering::DrawFilledCircle(renderer, reg->softbodies[e].massPoints[p].position.x, reg->softbodies[e].massPoints[p].position.y, 10, SDL_Color{ 255, 0, 0, 1 });
			}
		}
	}
}
