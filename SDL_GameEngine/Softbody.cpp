#include "Softbody.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include <algorithm>
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
			for (int s = 0; s < reg->softbodies[e].springs.size(); s++)
			{
				reg->softbodies[e].springs[s].springVelA = Vector2D(0, 0);
				reg->softbodies[e].springs[s].springVelB = Vector2D(0, 0);
			}
		}
	}
}
//void Softbody_System::ResolveCollision(MassPoint* A, Rigidbody_Component* B, Vector2D normal)
//{
//	//Calculate relative velocity between two objects -- improve by determining total velocity (linear + rotational velocity)
//	Vector2D relativeVelocity = B->velocity - A->velocity;
//
//	Vector2D force;
//
//	if (A->isStatic && !B->isStatic)
//	{
//		float j = Vector2D::DotProduct((relativeVelocity) * -(1 + A->elasticity), normal) / (1 / B->mass);
//		force = normal * j;
//		B->ApplyForce(force);
//	}
//	else if (B->isStatic && !A->isStatic)
//	{
//		float j = Vector2D::DotProduct((relativeVelocity) * -(1 + A->elasticity), normal) / ((1 / A->mass));
//		force = normal * j;
//		A->ApplyForce(force * -1);
//	}
//	else
//	{
//		float j = Vector2D::DotProduct((relativeVelocity) * -(1 + A->elasticity), normal) / ((1 / A->mass) + (1 / B->mass));
//		force = normal * j;
//		A->ApplyForce(force * -1);
//		B->ApplyForce(force);
//	}
//}
void Softbody_System::Update(Registry* reg, double* deltaTime)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				if (reg->softbodies[e].massPoints[p].isStatic) continue;

				reg->softbodies[e].massPoints[p].force = Vector2D(0, 0);
				reg->softbodies[e].massPoints[p].force.y += *reg->softbodies[e].gravity;

				

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
							
						}
					}
				}
			}
			for (int s = 0; s < reg->softbodies[e].springs.size(); s++)
			{
				
				Vector2D forceA, forceB;
				CalculateSpringForce(&reg->softbodies[e].springs[s], &reg->softbodies[e], &forceA, &forceB, deltaTime);
				reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].A].force += forceA;
				reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].B].force += forceB;
			}
			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				if (reg->softbodies[e].massPoints[p].isStatic) continue;

				reg->softbodies[e].massPoints[p].velocity += reg->softbodies[e].massPoints[p].force * *deltaTime;
				std::cout << reg->softbodies[e].massPoints[p].velocity.x << " " << reg->softbodies[e].massPoints[p].velocity.y << std::endl;

				reg->softbodies[e].massPoints[p].position += reg->softbodies[e].massPoints[p].velocity * *deltaTime;
			}
		}
	}
}
float f1;
Vector2D Softbody_System::CalculateSpringForce(Spring * s, Softbody_Component * c,  Vector2D* forceA, Vector2D* forceB, double* deltaTime)
{
	float length = (c->massPoints[s->B].position - c->massPoints[s->A].position).length();
	float f = s->stiffness * (length - s->restLength);
	f1 = length;

	Vector2D forceToBeAppliedOnA = (c->massPoints[s->B].position - c->massPoints[s->A].position).normalize() * f;
	Vector2D forceToBeAppliedOnB = (c->massPoints[s->A].position - c->massPoints[s->B].position).normalize() * f;

	s->springVelA += forceToBeAppliedOnA * *deltaTime;
	s->springVelB += forceToBeAppliedOnB * *deltaTime;


	*forceA = forceToBeAppliedOnA - s->springVelA * s->dampingFactor;
	*forceB = forceToBeAppliedOnB - s->springVelB * s->dampingFactor;

	
	return forceToBeAppliedOnA;
}

void Softbody_System::Draw(Registry* reg, SDL_Renderer* renderer)
{
	
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				Uint8 x = 100 + 100 * p;
				ShapesRendering::DrawFilledCircle(renderer, reg->softbodies[e].massPoints[p].position.x, reg->softbodies[e].massPoints[p].position.y, 10, SDL_Color{ x, 0, 0, 1 });
			}
			for (int s = 0; s < reg->softbodies[e].springs.size(); s++)
			{
				if(f1 > 0)
					SDL_SetRenderDrawColor(renderer, std::clamp<float>(f1, 0, 255), 0, 0, 1);
				else
					SDL_SetRenderDrawColor(renderer, 0, std::abs(std::clamp<float>(f1, -255, 0)), 0, 1);
				SDL_RenderSetScale(renderer, 2, 2);
				SDL_RenderDrawLine(renderer, reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].A].position.x / 2, reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].A].position.y / 2,
					reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].B].position.x / 2, reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].B].position.y / 2);
				SDL_RenderSetScale(renderer, 1, 1);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
			}
		}
	}
}
