#include "Softbody.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include <algorithm>

#define ComputeVelocityForCircles(v1, v2, x1, x2, m1, m2, out)\
{\
	float length = (x1 - x2).length(); \
	*out = v1 - Vector2D((2 * m2 / (m1 + m2)) * Vector2D::DotProduct(v1 - v2, x1 - x2) / ((length == 0 ? 1 : length) * (length == 0 ? 1 : length))* (x1 - x2).x, (2 * m2 / (m1 + m2)) * Vector2D::DotProduct(v1 - v2, x1 - x2) / ((length == 0 ? 1 : length) * (length == 0 ? 1 : length))* (x1 - x2).y);\
}\

void MassPoint::ApplyForce(Vector2D force)
{
	if (!isStatic)
	{
		//Use GetMass() and GetMoment() here in case we ever need them to do something more than just return their values
		//Calculate the object's acceleration
		
		//Add acceleration to object's velocity
		this->force += force;
	}
}


void Softbody_System::Init(Registry* reg)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
			if (reg->softbodies[e].springs.size() <= 0)
			{
				for (int x = 0; x < reg->softbodies[e].massPoints.size(); x++)
				{
					for (int y = x + 1; y < reg->softbodies[e].massPoints.size(); y++)
					{
						float dist = (reg->softbodies[e].massPoints[y].position - reg->softbodies[e].massPoints[x].position).length();
						reg->softbodies[e].springs.push_back(Spring{ x, y, reg->softbodies[e].defaultStiffness, dist, reg->softbodies[e].defaultDampingFactor });
					}
				}
			}

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
				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (e == e2) continue;

					if (reg->softbodies.count(e2))
					{
						int intersectionCount = 0;
						float currentDistanceFromClosest = 0;
						Vector2D closestPoint;
						MassPoint* lineOne = nullptr;
						MassPoint* lineTwo = nullptr;
						for (int i = 0; i < reg->softbodies[e2].massPoints.size(); i++)
						{
							Vector2D* X = &reg->softbodies[e2].massPoints[i].position;
							Vector2D* Y;
							int nextIndex = 0;
							if (i == reg->softbodies[e2].massPoints.size() - 1)
							{
								Y = &reg->softbodies[e2].massPoints[0].position;
								nextIndex = 0;
							}
							else
							{
								Y = &reg->softbodies[e2].massPoints[i + 1].position;
								nextIndex = i + 1;
							}

							Vector2D closestPointThis = ColliderFunctions::ClosestPointToLine(X, Y, &reg->softbodies[e].massPoints[p].position);
							Vector2D dir = closestPointThis - reg->softbodies[e].massPoints[p].position;
							float dist = dir.x * dir.x + dir.y * dir.y;
							if (currentDistanceFromClosest > dist || i == 0)
							{
								currentDistanceFromClosest = dist;
								closestPoint = closestPointThis;
								lineOne = &reg->softbodies[e2].massPoints[i];
								lineTwo = &reg->softbodies[e2].massPoints[nextIndex];
							}

							if (ColliderFunctions::LineLineIntersection(reg->softbodies[e].massPoints[p].position, reg->softbodies[e].massPoints[p].position + Vector2D(1000000, 0), *X, *Y))
							{
								intersectionCount++;
							}
						}
						if (intersectionCount % 2 != 0)
						{
							Vector2D reversedNormal = ((closestPoint - reg->softbodies[e].massPoints[p].position) * -1).normalize();
							Vector2D x2 = reg->softbodies[e].massPoints[p].position + reversedNormal;
							Vector2D v2 = (lineOne->velocity + lineTwo->velocity) / 2;

							Vector2D newV1, newV2;
							ComputeVelocityForCircles(reg->softbodies[e].massPoints[p].velocity, v2, reg->softbodies[e].massPoints[p].position, x2, 1, 1, &newV1);
							ComputeVelocityForCircles(v2, reg->softbodies[e].massPoints[p].velocity, x2, reg->softbodies[e].massPoints[p].position, 1, 1, &newV2);

							reg->softbodies[e].massPoints[p].velocity = newV1;
							if(!lineOne->isStatic)lineOne->velocity = newV2;
							if(!lineTwo->isStatic)lineTwo->velocity = newV2;

							

							
							std::cout << reg->softbodies[e].massPoints[p].velocity.x << " " << reg->softbodies[e].massPoints[p].velocity.y << std::endl;
							reg->softbodies[e].massPoints[p].position = closestPoint;
							
							
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

				float velLength = reg->softbodies[e].massPoints[p].velocity.length();
				
				Vector2D dragForce = reg->softbodies[e].massPoints[p].velocity / (velLength == 0 ? 1 : velLength) * -1 * *reg->softbodies[e].dragCoeficient * velLength * velLength;

				reg->softbodies[e].massPoints[p].force += dragForce;

				reg->softbodies[e].massPoints[p].velocity += reg->softbodies[e].massPoints[p].force * *deltaTime;
				
				reg->softbodies[e].massPoints[p].position += reg->softbodies[e].massPoints[p].velocity * *deltaTime;
			}
		}
	}
}
void Softbody_System::CalculateSpringForce(Spring * s, Softbody_Component * c,  Vector2D* forceA, Vector2D* forceB, double* deltaTime)
{
	float length = (c->massPoints[s->B].position - c->massPoints[s->A].position).length();
	float f = s->stiffness * (length - s->restLength);
#ifdef _DEBUG
	s->f = f;
#endif // _DEBUG


	Vector2D normalizedDir = (c->massPoints[s->B].position - c->massPoints[s->A].position) / (length == 0 ? 1 : length);
	Vector2D velDif = c->massPoints[s->B].velocity - c->massPoints[s->A].velocity;

	float dot = Vector2D::DotProduct(normalizedDir, velDif);
	float damp = dot * s->dampingFactor;

	float fullForce = f + damp;

	*forceA = normalizedDir * fullForce;
	*forceB = normalizedDir * -1 * fullForce ;

	s->springVelA += *forceA * *deltaTime;
	s->springVelB += *forceB * *deltaTime;
}
void Softbody_System::ResolveCollision(MassPoint* A, MassPoint* B, MassPoint* P, Vector2D normal)
{
	//Calculate relative velocity between two objects -- improve by determining total velocity (linear + rotational velocity)
	Vector2D relativeVelocity = (A->velocity + B->velocity) / 2 - P->velocity;

	Vector2D force;

	float j = Vector2D::DotProduct((relativeVelocity) * -2, normal) / 2;
	force = normal * j;
	P->ApplyForce(force * -1);
	B->ApplyForce(force);
	A->ApplyForce(force);
}

void Softbody_System::Draw(Registry* reg, SDL_Renderer* renderer)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
#ifdef _DEBUG
			for (int s = 0; s < reg->softbodies[e].springs.size(); s++)
			{
				


				SDL_SetRenderDrawColor(renderer, std::clamp<float>(reg->softbodies[e].springs[s].f, 0, 255), 0, 0, 1);
				
				SDL_RenderSetScale(renderer, 2, 2);
				SDL_RenderDrawLine(renderer, reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].A].position.x / 2, reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].A].position.y / 2,
					reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].B].position.x / 2, reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].B].position.y / 2);
				SDL_RenderSetScale(renderer, 1, 1);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
			}
#endif // _DEBUG
			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				ShapesRendering::DrawFilledCircle(renderer, reg->softbodies[e].massPoints[p].position.x, reg->softbodies[e].massPoints[p].position.y, 10, SDL_Color{ 255, 0, 0, 1 });
			}
		}
	}
}
