#include "Softbody.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include <algorithm>
#include "SDL2_gfxPrimitives.h"
#include <cmath>

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
			reg->softbodies[e].x = new Sint16[reg->softbodies[e].massPointsN];
			reg->softbodies[e].y = new Sint16[reg->softbodies[e].massPointsN];

			if (reg->softbodies[e].hardShapeMatching)
			{
				reg->softbodies[e].xFrame = new Sint16[reg->softbodies[e].massPointsN];
				reg->softbodies[e].yFrame = new Sint16[reg->softbodies[e].massPointsN];
			}

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
				if (reg->softbodies[e].hardShapeMatching)
				{

					Vector2D averagePosition = Vector2D(0, 0); //Position of the hard frame
					for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
					{
						averagePosition += reg->softbodies[e].massPoints[p].position;
					}
					averagePosition = averagePosition / reg->softbodies[e].massPoints.size();
					reg->softbodies[e].originalPositionsOfMassPoints.push_back(reg->softbodies[e].massPoints[p].position - averagePosition);
					reg->softbodies[e].springsForFrame.push_back(Spring{ p, p, reg->softbodies[e].defaultStiffness / 20, 0, reg->softbodies[e].defaultDampingFactor });
				}
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
inline float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

Vector2D closestPointX;
void Softbody_System::Update(Registry* reg, double* deltaTime)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
			reg->softbodies[e].closestPoints.clear();
			reg->softbodies[e].finalPositionsOfHardFrame.clear();

#pragma region Collision

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
							reg->softbodies[e].closestPoints.push_back(closestPointThis);
							Vector2D dir = closestPointThis - reg->softbodies[e].massPoints[p].position;
							float dist = dir.x * dir.x + dir.y * dir.y;
							if (currentDistanceFromClosest > dist || i == 0)
							{
								currentDistanceFromClosest = dist;
								closestPoint = closestPointThis;
								lineOne = &reg->softbodies[e2].massPoints[i];
								lineTwo = &reg->softbodies[e2].massPoints[nextIndex];
							}

							if (ColliderFunctions::LineLineIntersection(reg->softbodies[e].massPoints[p].position + reg->softbodies[e].massPoints[p].velocity * *deltaTime, reg->softbodies[e].massPoints[p].position + reg->softbodies[e].massPoints[p].velocity * *deltaTime + Vector2D(1000000, 0), *X, *Y))
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
						
							if (!lineOne->isStatic && !lineTwo->isStatic)
							{
								Vector2D dir = (closestPoint - reg->softbodies[e].massPoints[p].position) / 2;
								reg->softbodies[e].massPoints[p].position += dir;
								float dist = (lineTwo->position - lineOne->position).length();
								float distDif = (closestPoint - lineOne->position).length();
								float per = distDif / dist;
								
								lineOne->position -= dir * (1 - per);
								lineTwo->position -= dir * per;
							}
							else if (lineOne->isStatic && !lineTwo->isStatic)
							{
								Vector2D dir = (closestPoint - reg->softbodies[e].massPoints[p].position) / 2;
								reg->softbodies[e].massPoints[p].position += dir;
								lineTwo->position -= dir;
							}
							else if (!lineOne->isStatic && lineTwo->isStatic)
							{
								Vector2D dir = (closestPoint - reg->softbodies[e].massPoints[p].position) / 2;
								reg->softbodies[e].massPoints[p].position += dir;
								lineOne->position -= dir;
							}
							else
							{
								reg->softbodies[e].massPoints[p].position = closestPoint;
							}
						}
					}
				}
			}

#pragma endregion

			
#pragma region ShapeMatching
	
			if (reg->softbodies[e].hardShapeMatching)
			{
				Vector2D averagePosition = Vector2D(0, 0); //Position of the hard frame
				for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
				{
					averagePosition += reg->softbodies[e].massPoints[p].position;
					
				}
				averagePosition = averagePosition / reg->softbodies[e].massPoints.size();

				float averageAngle = 0;
				std::vector<Vector2D> positionsNotAngled;
				for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
				{
					positionsNotAngled.push_back(reg->softbodies[e].originalPositionsOfMassPoints[p]);
					averageAngle = std::atan2(reg->softbodies[e].originalPositionsOfMassPoints[p].y - reg->softbodies[e].massPoints[p].position.y - averagePosition.y,
						reg->softbodies[e].massPoints[p].position.x - averagePosition.x - reg->softbodies[e].originalPositionsOfMassPoints[p].x);
				}
				std::cout << averageAngle << std::endl;

				for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
				{
					reg->softbodies[e].finalPositionsOfHardFrame.push_back(Vector2D(cos(0.1)* reg->softbodies[e].originalPositionsOfMassPoints[p].x - sin(0.1) * reg->softbodies[e].originalPositionsOfMassPoints[p].y,
						cos(0.1) * reg->softbodies[e].originalPositionsOfMassPoints[p].y + sin(0.1) * reg->softbodies[e].originalPositionsOfMassPoints[p].x) + averagePosition);
					reg->softbodies[e].xFrame[p] = reg->softbodies[e].finalPositionsOfHardFrame[p].x;
					reg->softbodies[e].yFrame[p] = reg->softbodies[e].finalPositionsOfHardFrame[p].y;
				}
				
				reg->softbodies[e].lastFrameAngle = averageAngle;
			}

#pragma endregion

#pragma region SpringForce

			for (int s = 0; s < reg->softbodies[e].springs.size(); s++)
			{
				
				Vector2D forceA, forceB;
				CalculateSpringForce(&reg->softbodies[e].springs[s], &reg->softbodies[e], &forceA, &forceB, deltaTime);
				reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].A].force += forceA;
				reg->softbodies[e].massPoints[reg->softbodies[e].springs[s].B].force += forceB;
			}
			for (int s = 0; s < reg->softbodies[e].springsForFrame.size(); s++)
			{

				Vector2D forceA;
				CalculateSpringForceForFrame(&reg->softbodies[e].springsForFrame[s], &reg->softbodies[e], &forceA, deltaTime, reg->softbodies[e].finalPositionsOfHardFrame[reg->softbodies[e].springsForFrame[s].A]);
				reg->softbodies[e].massPoints[reg->softbodies[e].springsForFrame[s].A].force += forceA;
			}

#pragma endregion

#pragma region AddAllForcesTogetherWithDragForce

			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				if (!reg->softbodies[e].massPoints[p].isStatic)
				{

					float velLength = reg->softbodies[e].massPoints[p].velocity.length();

					Vector2D dragForce = reg->softbodies[e].massPoints[p].velocity / (velLength == 0 ? 1 : velLength) * -1 * *reg->softbodies[e].dragCoeficient * velLength * velLength;

					reg->softbodies[e].massPoints[p].force += dragForce;

					reg->softbodies[e].massPoints[p].velocity += reg->softbodies[e].massPoints[p].force * *deltaTime;

					reg->softbodies[e].massPoints[p].position += reg->softbodies[e].massPoints[p].velocity * *deltaTime;
				}

				reg->softbodies[e].x[p] = reg->softbodies[e].massPoints[p].position.x;
				reg->softbodies[e].y[p] = reg->softbodies[e].massPoints[p].position.y;
			}

#pragma endregion

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
void Softbody_System::CalculateSpringForceForFrame(Spring* s, Softbody_Component* c, Vector2D* forceA, double* deltaTime, Vector2D framePosition)
{
	float length = (framePosition - c->massPoints[s->A].position).length();
	float f = s->stiffness * (length - s->restLength);
#ifdef _DEBUG
	s->f = f;
#endif // _DEBUG


	Vector2D normalizedDir = (framePosition - c->massPoints[s->A].position) / (length == 0 ? 1 : length);
	Vector2D velDif = Vector2D(0, 0) - c->massPoints[s->A].velocity;

	float dot = Vector2D::DotProduct(normalizedDir, velDif);
	float damp = dot * s->dampingFactor;

	float fullForce = f + damp;

	*forceA = normalizedDir * fullForce;

	s->springVelA += *forceA * *deltaTime;
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
#ifdef _DEBUG


	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodies.count(e))
		{
			
			filledPolygonRGBA(renderer, reg->softbodies[e].x, reg->softbodies[e].y, reg->softbodies[e].massPointsN, 255, 0, 0, 255);
			if (reg->softbodies[e].hardShapeMatching)
			{
				polygonRGBA(renderer, reg->softbodies[e].xFrame, reg->softbodies[e].yFrame, reg->softbodies[e].massPointsN, 0, 255, 0, 255);
			}


			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				filledCircleRGBA(renderer, reg->softbodies[e].massPoints[p].position.x, reg->softbodies[e].massPoints[p].position.y, 10, 255, 0, 0, 255 );
			}
		}
	}
#endif // _DEBUG
}
