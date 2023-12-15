#include "Softbody.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include <algorithm>
#include "SDL2_gfxPrimitives.h"
#include "Sdl_image.h"
#include <cmath>
#include "Game.h"

namespace Engine
{
#define ComputeVelocityForCircles(v1, v2, x1, x2, m1, m2, out)\
{\
	double length = (x1 - x2).length(); \
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

	void Softbody_System::Init(Registry* reg, Game* game)
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
							double dist = (reg->softbodies[e].massPoints[y].position - reg->softbodies[e].massPoints[x].position).length();
							reg->softbodies[e].springs.push_back(Spring{ x, y, reg->softbodies[e].defaultStiffness, dist, reg->softbodies[e].defaultDampingFactor });
						}
					}
				}

				for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
				{
					reg->softbodies[e].massPoints[p].velocity = Vector2D(0, 0);
					if (reg->softbodies[e].hardShapeMatching)
					{
						reg->softbodies[e].averagePosition = Vector2D(0, 0); //Position of the hard frame
						for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
						{
							reg->softbodies[e].averagePosition += reg->softbodies[e].massPoints[p].position;
						}
						reg->softbodies[e].averagePosition = reg->softbodies[e].averagePosition / reg->softbodies[e].massPoints.size();
						reg->softbodies[e].originalPositionsOfMassPoints.push_back(reg->softbodies[e].massPoints[p].position - reg->softbodies[e].averagePosition);
						reg->softbodies[e].springsForFrame.push_back(Spring{ p, p, reg->softbodies[e].shapeMatchingStiffness, 0, reg->softbodies[e].shapeMatchingDampingFactor });
					}
				}
				if (reg->transforms.count(e))reg->transforms[e].position = reg->softbodies[e].averagePosition;
				for (int s = 0; s < reg->softbodies[e].springs.size(); s++)
				{
					reg->softbodies[e].springs[s].springVelA = Vector2D(0, 0);
					reg->softbodies[e].springs[s].springVelB = Vector2D(0, 0);
				}
			}
		}
	}

	void Softbody_System::StartUpdate(Registry* reg)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->softbodies.count(e))
			{
				for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
				{
					reg->softbodies[e].massPoints[p].force = Vector2D(0, 0);
				}
			}
		}
	}

	inline double get_angle_2points(Vector2D p1, Vector2D p2)
	{
		//Make point1 the origin, make point2 relative to the origin so we do point1 - point1, and point2-point1,
		//since we dont need point1 for the equation to work, the equation works correctly with the origin 0,0.
		int deltaY = p2.y - p1.y;
		int deltaX = p2.x - p1.x; //Vector 2 is now relative to origin, the angle is the same, we have just transformed it to use the origin.

		double angleInRadians = atan2(Vector2D::CrossDotProduct(p1, p2), Vector2D::DotProduct(p1, p2));

		return angleInRadians;
	}

	inline double areaOfPolygon(std::vector<MassPoint> points, int n)
	{
		double area = 0.0;

		// Calculate value of shoelace formula
		int j = n - 1;
		for (int i = 0; i < n; i++)
		{
			area += (points[j].position.x + points[i].position.x) * (points[j].position.y - points[i].position.y);
			j = i;  // j is previous vertex to i
		}

		// Return absolute value
		return abs(area / 2.0);
	}

	
	void Softbody_System::Update(Registry* reg, double* deltaTime, Game* game)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->softbodies.count(e))
			{
				reg->softbodies[e].closestPoints.clear();
				reg->softbodies[e].finalPositionsOfHardFrame.clear();

				if (reg->transforms.count(e) || reg->softbodies[e].hardShapeMatching || reg->softbodies[e].gasShapeMatching)
				{
					reg->softbodies[e].averagePosition = Vector2D(0, 0); //Position of the hard frame
					for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
					{
						reg->softbodies[e].averagePosition += reg->softbodies[e].massPoints[p].position;
					}
					reg->softbodies[e].averagePosition = reg->softbodies[e].averagePosition / reg->softbodies[e].massPoints.size();

					if (reg->transforms.count(e)) reg->transforms[e].position = reg->softbodies[e].averagePosition;
				}

#pragma region Collision

				for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
				{
					reg->softbodies[e].massPoints[p].force.y += *reg->softbodies[e].gravity * reg->softbodies[e].massPoints[p].mass * reg->softbodies[e].massPoints[p].gravityMultiplayer;

					SDL_Point A = { reg->softbodies[e].massPoints[p].position.x, reg->softbodies[e].massPoints[p].position.y };

					for (int e2 = 0; e2 <= EntityManager::Instance()->num_entities; e2++)
					{
						if (reg->AABBColliders.count(e2) && reg->rigidbodies.count(e2))
						{
							if ((reg->rigidbodies[e2].ignoreLayers & reg->softbodies[e].layer) || (reg->softbodies[e].ignoreLayers & reg->rigidbodies[e2].layer)) continue;

							SDL_Rect r = { reg->AABBColliders[e2].position->x, reg->AABBColliders[e2].position->y, reg->AABBColliders[e2].width, reg->AABBColliders[e2].height };
							if (SDL_PointInRect(&A, &r))
							{
								Vector2D min = *reg->AABBColliders[e2].position;
								Vector2D max = *reg->AABBColliders[e2].position + Vector2D(reg->AABBColliders[e2].width, reg->AABBColliders[e2].height);

								double yTop = std::abs(A.y - min.y);
								double yDown = std::abs(A.y - max.y);
								double xRight = std::abs(A.x - max.y);
								double xLeft = std::abs(A.x - min.y);

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
							if (!(reg->softbodies[e2].ignoreLayers & reg->softbodies[e].layer) && !(reg->softbodies[e].ignoreLayers & reg->softbodies[e2].layer))
							{
								int intersectionCountX = 0;
								int intersectionCountY = 0;
								double currentDistanceFromClosest = 0;
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
									double dist = dir.x * dir.x + dir.y * dir.y;
									if (currentDistanceFromClosest > dist || i == 0)
									{
										currentDistanceFromClosest = dist;
										closestPoint = closestPointThis;
										lineOne = &reg->softbodies[e2].massPoints[i];
										lineTwo = &reg->softbodies[e2].massPoints[nextIndex];
									}

									if (ColliderFunctions::LineLineIntersection(reg->softbodies[e].massPoints[p].position, reg->softbodies[e].massPoints[p].position + Vector2D(1000000, 0), *X, *Y))
									{
										intersectionCountX++;
									}
									if (ColliderFunctions::LineLineIntersection(reg->softbodies[e].massPoints[p].position, reg->softbodies[e].massPoints[p].position + Vector2D(0, 1000000), *X, *Y))
									{
										intersectionCountY++;
									}
								}
								if (intersectionCountX % 2 != 0 && intersectionCountY % 2 != 0)
								{
									Vector2D reversedNormal = ((closestPoint - reg->softbodies[e].massPoints[p].position) * -1).normalize();
									Vector2D x2 = reg->softbodies[e].massPoints[p].position + reversedNormal;
									Vector2D v2 = (lineOne->velocity + lineTwo->velocity) / 2;

									Vector2D newV1, newV2;
									double averageFriction = (reg->softbodies[e].friction + reg->softbodies[e2].friction) / 2;
									ComputeVelocityForCircles(reg->softbodies[e].massPoints[p].velocity, v2, reg->softbodies[e].massPoints[p].position, x2, reg->softbodies[e].massPoints[p].mass, ((lineOne->mass + lineTwo->mass) / 2), &newV1);
									ComputeVelocityForCircles(v2, reg->softbodies[e].massPoints[p].velocity, x2, reg->softbodies[e].massPoints[p].position, ((lineOne->mass + lineTwo->mass) / 2), reg->softbodies[e].massPoints[p].mass, &newV2);

									Vector2D norm = reversedNormal * -newV1.length();
									Vector2D dif = norm - newV1;

									Vector2D norm2 = reversedNormal * newV2.length();
									Vector2D dif2 = norm2 - newV2;

									if (!reg->softbodies[e].massPoints[p].isStatic)reg->softbodies[e].massPoints[p].velocity = (newV1 + dif * averageFriction) * reg->softbodies[e].bounciness;
									if (!lineOne->isStatic)lineOne->velocity = (newV2 + dif2 * averageFriction) * reg->softbodies[e2].bounciness;
									if (!lineTwo->isStatic)lineTwo->velocity = (newV2 + dif2 * averageFriction) * reg->softbodies[e2].bounciness;

									if (!lineOne->isStatic && !lineTwo->isStatic)
									{
										Vector2D dir = (closestPoint - reg->softbodies[e].massPoints[p].position) / 2;
										if (!reg->softbodies[e].massPoints[p].isStatic)reg->softbodies[e].massPoints[p].position += dir;
										double dist = (lineTwo->position - lineOne->position).length();
										double distDif = (closestPoint - lineOne->position).length();
										double per = distDif / dist;

										lineOne->position -= dir * (per);
										lineTwo->position -= dir * (1 - per);
									}
									else if (lineOne->isStatic && !lineTwo->isStatic)
									{
										Vector2D dir = (closestPoint - reg->softbodies[e].massPoints[p].position) / 2;
										if (!reg->softbodies[e].massPoints[p].isStatic)reg->softbodies[e].massPoints[p].position += dir;
										lineTwo->position -= dir;
									}
									else if (!lineOne->isStatic && lineTwo->isStatic)
									{
										Vector2D dir = (closestPoint - reg->softbodies[e].massPoints[p].position) / 2;
										if (!reg->softbodies[e].massPoints[p].isStatic)reg->softbodies[e].massPoints[p].position += dir;
										lineOne->position -= dir;
									}
									else
									{
										if (!reg->softbodies[e].massPoints[p].isStatic)reg->softbodies[e].massPoints[p].position = closestPoint;
									}
								}
							}
						}
					}
				}

#pragma endregion

#pragma region ShapeMatching

				if (reg->softbodies[e].hardShapeMatching)
				{
					if (reg->softbodies[e].rotateHardFrame)
					{
						double averageAngleCos = 0;
						double averageAngleSin = 0;
						for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
						{
							double angle = get_angle_2points(reg->softbodies[e].originalPositionsOfMassPoints[p], (reg->softbodies[e].massPoints[p].position - reg->softbodies[e].averagePosition));
							averageAngleCos += cos(angle);
							averageAngleSin += sin(angle);
						}
						double averageAngle = atan2(averageAngleSin, averageAngleCos);

						for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
						{
							reg->softbodies[e].finalPositionsOfHardFrame.push_back(Vector2D(cos(averageAngle) * reg->softbodies[e].originalPositionsOfMassPoints[p].x - sin(averageAngle) * reg->softbodies[e].originalPositionsOfMassPoints[p].y,
								cos(averageAngle) * reg->softbodies[e].originalPositionsOfMassPoints[p].y + sin(averageAngle) * reg->softbodies[e].originalPositionsOfMassPoints[p].x) + reg->softbodies[e].averagePosition);
							reg->softbodies[e].xFrame[p] = reg->softbodies[e].finalPositionsOfHardFrame[p].x;
							reg->softbodies[e].yFrame[p] = reg->softbodies[e].finalPositionsOfHardFrame[p].y;
						}
						reg->softbodies[e].lastFrameAverageAngle = averageAngle;
					}
					else
					{
						for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
						{
							reg->softbodies[e].finalPositionsOfHardFrame.push_back(reg->softbodies[e].originalPositionsOfMassPoints[p] + reg->softbodies[e].averagePosition);
						}
					}
				}
				if (reg->softbodies[e].gasShapeMatching)
				{
					for (int i = 0; i < reg->softbodies[e].massPointsN; i++)
					{
						int nextIndex = 0;
						if (i == reg->softbodies[e].massPointsN - 1)
						{
							nextIndex = 0;
						}
						else
						{
							nextIndex = i + 1;
						}

						int beforeIndex = 0;
						if (i == 0)
						{
							beforeIndex = reg->softbodies[e].massPointsN - 1;
						}
						else
						{
							beforeIndex = i - 1;
						}
						Vector2D A = reg->softbodies[e].massPoints[beforeIndex].position;
						Vector2D B = reg->softbodies[e].massPoints[nextIndex].position;
						Vector2D normal = ColliderFunctions::ReflectionNormal(A, B, reg->softbodies[e].averagePosition);

						double area = areaOfPolygon(reg->softbodies[e].massPoints, reg->softbodies[e].massPointsN);
						std::cout << "Pressure = " << area << std::endl;
						float dist = (reg->softbodies[e].massPoints[i].position - reg->softbodies[e].averagePosition).length();
						double p = dist * reg->softbodies[e].shapeMatchingGasAmount * (1.0f / area);
						reg->softbodies[e].massPoints[i].force += normal * p;
					}
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
				if (reg->softbodies[e].hardShapeMatching)
				{
					for (int s = 0; s < reg->softbodies[e].springsForFrame.size(); s++)
					{
						Vector2D forceA;
						CalculateSpringForceForFrame(&reg->softbodies[e].springsForFrame[s], &reg->softbodies[e], &forceA, deltaTime, reg->softbodies[e].finalPositionsOfHardFrame[reg->softbodies[e].springsForFrame[s].A]);
						reg->softbodies[e].massPoints[reg->softbodies[e].springsForFrame[s].A].force += forceA;
					}
				}

#pragma endregion

#pragma region AddAllForcesTogetherWithDragForce

			for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
			{
				if (!reg->softbodies[e].massPoints[p].isStatic)
				{
					double velLength = reg->softbodies[e].massPoints[p].velocity.length();

					Vector2D dragForce = reg->softbodies[e].massPoints[p].velocity / (velLength == 0 ? 1 : velLength) * -1 * *reg->softbodies[e].dragCoeficient * velLength * velLength;

					reg->softbodies[e].massPoints[p].force += dragForce;

					reg->softbodies[e].massPoints[p].velocity += reg->softbodies[e].massPoints[p].force * *deltaTime;

					reg->softbodies[e].massPoints[p].position += reg->softbodies[e].massPoints[p].velocity * *deltaTime;
					/*if (reg->softbodies[e].massPoints[e].position.x ? || isnan(reg->softbodies[e].massPoints[e].position.y))
					{
						std::cout << "awdoawubdoiawbfjlehf eljh felifhbef\n";
						EntityManager::Instance()->DestroyEntity(e);
					}*/
				}

				if (game->cam != nullptr)
				{
					reg->softbodies[e].x[p] = (reg->softbodies[e].massPoints[p].position.x - game->cam->resultRect.x) / *game->cam->size + ((game->cam->w * *game->cam->size - game->cam->w) / 2) / *game->cam->size;
					reg->softbodies[e].y[p] = (reg->softbodies[e].massPoints[p].position.y - game->cam->resultRect.y) / *game->cam->size + ((game->cam->h * *game->cam->size - game->cam->h) / 2) / *game->cam->size;
				}
			}

#pragma endregion
			}
		}
	}
	void Softbody_System::CalculateSpringForce(Spring* s, Softbody_Component* c, Vector2D* forceA, Vector2D* forceB, double* deltaTime)
	{
		double length = (c->massPoints[s->B].position - c->massPoints[s->A].position).length();
		double f = s->stiffness * (length - s->restLength);

		Vector2D normalizedDir = (c->massPoints[s->B].position - c->massPoints[s->A].position) / (length == 0 ? 1 : length);
		Vector2D velDif = c->massPoints[s->B].velocity + normalizedDir * f * -1 * *deltaTime - c->massPoints[s->A].velocity + normalizedDir * f * *deltaTime;

		double dot = Vector2D::DotProduct(normalizedDir, velDif);
		double damp = dot * s->dampingFactor;

		double fullForce = f + damp;

		*forceA = normalizedDir * fullForce;
		*forceB = normalizedDir * -1 * fullForce;

		s->springVelA += *forceA * *deltaTime;
		s->springVelB += *forceB * *deltaTime;
	}
	void Softbody_System::CalculateSpringForceForFrame(Spring* s, Softbody_Component* c, Vector2D* forceA, double* deltaTime, Vector2D framePosition)
	{
		double length = (framePosition - c->massPoints[s->A].position).length();
		double f = s->stiffness * (length - s->restLength);

		Vector2D normalizedDir = (framePosition - c->massPoints[s->A].position) / (length == 0 ? 1 : length);
		Vector2D velDif = Vector2D(0, 0) - c->massPoints[s->A].velocity + normalizedDir * f * *deltaTime;

		double dot = Vector2D::DotProduct(normalizedDir, velDif);
		double damp = dot * s->dampingFactor;

		double fullForce = f + damp;

		*forceA = normalizedDir * fullForce;

		s->springVelA += *forceA * *deltaTime;
	}
	void Softbody_System::ResolveCollision(MassPoint* A, MassPoint* B, MassPoint* P, Vector2D normal)
	{
		//Calculate relative velocity between two objects -- improve by determining total velocity (linear + rotational velocity)
		Vector2D relativeVelocity = (A->velocity + B->velocity) / 2 - P->velocity;

		Vector2D force;

		double j = Vector2D::DotProduct((relativeVelocity) * -2, normal) / 2;
		force = normal * j;
		P->ApplyForce(force * -1);
		B->ApplyForce(force);
		A->ApplyForce(force);
	}

	void Softbody_System::Draw(Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect, Game* game)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->softbodies.count(e))
			{
				filledPolygonRGBA(renderer, reg->softbodies[e].x, reg->softbodies[e].y, reg->softbodies[e].massPointsN, reg->softbodies[e].color.r, reg->softbodies[e].color.g, reg->softbodies[e].color.b, reg->softbodies[e].color.a);
				/*	if (reg->softbodies[e].hardShapeMatching)
					{
						polygonRGBA(renderer, reg->softbodies[e].xFrame, reg->softbodies[e].yFrame, reg->softbodies[e].massPointsN, 93, 147, 123, 255);
					}*/

				for (int p = 0; p < reg->softbodies[e].massPoints.size(); p++)
				{
					/*filledCircleRGBA(renderer, reg->softbodies[e].massPoints[p].position.x, reg->softbodies[e].massPoints[p].position.y, 10, 255, 0, 0, 255);*/
					int nextIndex = 0;
					if (p == reg->softbodies[e].massPoints.size() - 1)
					{
						nextIndex = 0;
					}
					else
					{
						nextIndex = p + 1;
					}
					if (game->cam != NULL)
					{
						thickLineRGBA(renderer, reg->softbodies[e].x[p], reg->softbodies[e].y[p], reg->softbodies[e].x[nextIndex], reg->softbodies[e].y[nextIndex], 10 / *game->cam->size, 0, 0, 0, 255);
					}
				}

				//if (reg->softbodies[e].hardShapeMatching)
				//{
				//	filledCircleRGBA(renderer, reg->softbodies[e].massPoints[reg->softbodies[e].massPoints.size() - 1].position.x, reg->softbodies[e].massPoints[reg->softbodies[e].massPoints.size() - 1].position.y, 10, 0, 0, 255, 255);
				//	filledCircleRGBA(renderer, reg->softbodies[e].averagePosition.x, reg->softbodies[e].averagePosition.y, 10, 0, 255, 0, 255);
				//	filledCircleRGBA(renderer, reg->softbodies[e].originalPositionsOfMassPoints[reg->softbodies[e].originalPositionsOfMassPoints.size() - 1].x + reg->softbodies[e].averagePosition.x, reg->softbodies[e].originalPositionsOfMassPoints[reg->softbodies[e].originalPositionsOfMassPoints.size() - 1].y + reg->softbodies[e].averagePosition.y, 10, 0, 255, 0, 255);
				//	filledCircleRGBA(renderer, reg->softbodies[e].finalPositionsOfHardFrame[reg->softbodies[e].finalPositionsOfHardFrame.size() - 1].x, reg->softbodies[e].finalPositionsOfHardFrame[reg->softbodies[e].finalPositionsOfHardFrame.size() - 1].y, 10, 0, 255, 0, 255);
				//}
			}
		}
	}
}