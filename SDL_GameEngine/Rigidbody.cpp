#include "Rigidbody.h"
#include "Entity.h"
#include "Registry.h"
#include <cmath>
#include <numeric>

namespace Engine
{
#define ComputeVelocityForCircles(v1, v2, x1, x2, m1, m2, out)\
 *out = v1 - Vector2D((2 * m2 / (m1 + m2)) * Vector2D::DotProduct(v1 - v2, x1 - x2) / ((x1 - x2).length() * (x1 - x2).length())* (x1 - x2).x, (2 * m2 / (m1 + m2)) * Vector2D::DotProduct(v1 - v2, x1 - x2) / ((x1 - x2).length() * (x1 - x2).length())* (x1 - x2).y)\

	void Rigidbody_Component::ApplyForce(Vector2D force)
	{
		if (!isStatic)
		{
			
			//Calculate the object's acceleration
			Vector2D acceleration = force / mass;
			//Add acceleration to object's velocity
			velocity += acceleration;
		}
	}

#pragma region System

	void Rigidbody_System::Init(Registry* reg)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->rigidbodies.count(e))
			{
				Rigidbody_Component* c = &reg->rigidbodies[e];

				c->inverseMass = 1 / c->mass;
				if (reg->transforms.count(e))
				{
					Transform_Component* cT = &reg->transforms[e];
					c->position = &cT->position;
				}
			}
		}
	}

	void Rigidbody_System::Render(Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->rigidbodies.count(e))
			{
				if (reg->AABBColliders.count(e))
				{
					/*SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					SDL_RenderDrawRect(renderer, new SDL_Rect{ (int)(reg->transforms[e].position.x + reg->AABBColliders[e].offsetX + 1.5), (int)(reg->transforms[e].position.y + reg->AABBColliders[e].offsetY + 1.5), (int)reg->AABBColliders[e].width, (int)reg->AABBColliders[e].height });
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);*/
				}
			}
		}
	}


	Vector2D Rigidbody_System::ReflectionResponse(Vector2D* normal, Vector2D* velocity)
	{
		// u = v * n * n * n
		double dot = Vector2D::DotProduct(*velocity, *normal);

		Vector2D u = Vector2D(dot * normal->x, dot * normal->y);
		Vector2D w = *velocity - u;
		// w = v - u
		// new v = w - u

		// (v - v * n * n * n) - v * n * n * n

		Vector2D result = w - u;

		return result;
	}

	void Rigidbody_System::ResolveCollision(Rigidbody_Component* A, Rigidbody_Component* B, Vector2D normal, double* deltaTime)
	{
		//Calculate relative velocity between two objects -- improve by determining total velocity (linear + rotational velocity)
		Vector2D relativeVelocity = B->velocity - A->velocity;

		Vector2D force;


		if (A->isStatic && !B->isStatic)
		{
			double j = Vector2D::DotProduct((relativeVelocity) * -(1 + B->elasticity), normal) / ((1 / B->mass));
			force = normal * j;
			B->ApplyForce(force);
		}
		else if (B->isStatic && !A->isStatic)
		{
			double j = Vector2D::DotProduct((relativeVelocity) * -(1 + A->elasticity), normal) / ((1 / A->mass));
			force = normal * j;
			A->ApplyForce(force * -1);
		}
		else
		{
			double j = Vector2D::DotProduct((relativeVelocity) * -(1 + A->elasticity), normal) / ((1 / A->mass) + (1 / B->mass));
			force = normal * j;
			A->ApplyForce(force * -1);
			B->ApplyForce(force);
		}
	}

	void Rigidbody_System::PositionalCorrection(Rigidbody_Component* A, Rigidbody_Component* B, double penetration, Vector2D collisionNormal)
	{
		*A->position += collisionNormal * penetration;
	}
	void Rigidbody_System::Update(Registry* reg, double* deltaTime)
	{
		
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->rigidbodies.count(e))
			{
				Rigidbody_Component* c = &reg->rigidbodies[e];
				if (c->isStatic) continue;

				//Rigidbody properties to make it look better
				double m1 = c->mass;
				c->ApplyForce(Vector2D(0, *c->gravity * m1 * *deltaTime));

				Vector2D v1 = c->velocity;
				Vector2D x1 = *c->position;
				Vector2D upcomingX1 = x1 + v1 * *deltaTime;

				//Update the basic velocity, acceleration is usually gravity

				if (reg->circleColliders.count(e))
				{
					CircleCollider_Component* cc = &reg->circleColliders[e];

					for (int e2 = e + 1; e2 <= EntityManager::Instance()->num_entities; e2++)
					{
						if (reg->rigidbodies.count(e2))
						{
							Rigidbody_Component* c2 = &reg->rigidbodies[e2];

							//Basic rigidbody properties again for pretty look
							Vector2D v2 = c2->velocity;
							Vector2D x2 = *c2->position;
							Vector2D upcomingX2 = x2 + v2 * *deltaTime;
							double m2 = c2->mass;

							if (!(c2->ignoreLayers & c->layer) && !(c->ignoreLayers & c2->layer))
							{
								if (reg->circleColliders.count(e2))
								{
									CircleCollider_Component* cc2 = &reg->circleColliders[e2];

									double penetration = 0;

									if (ColliderFunctions::CircleWithCircleIntersection(upcomingX1, cc2->radius, upcomingX2, cc->radius, &penetration))
									{
										ResolveCollision(c, c2, (upcomingX2 - upcomingX1).normalize(), deltaTime);
									}
								}
							}
						}
					}
					for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
					{
						if (reg->lineColliders.count(e2))
						{
							LineCollider_Component* cl = &reg->lineColliders[e2];

							Vector2D pointOfIntersection;
							bool intersects = ColliderFunctions::CircleWithLineIntersection(cl, *cc->position, cc->radius, &pointOfIntersection);
							if (intersects)
							{
								//Collided with line

								//Get the normal which is closer to the middle
								Vector2D normal = ColliderFunctions::ReflectionNormal(cl, *cc->position);
								//Calculate the response
								c->velocity = ReflectionResponse(&normal, &v1) * c->elasticity;
								//Make sure that the position is awway from line so that the collision doesn't happen twice
								*c->position = ColliderFunctions::PositionToReturnToAfterCollision(&normal, cc, pointOfIntersection);
							}
						}
						if (reg->rigidbodies.count(e2))
						{
							Rigidbody_Component* c2 = &reg->rigidbodies[e2];

							Vector2D v2 = c2->velocity;
							Vector2D x2 = *c2->position;
							Vector2D upcomingX2 = x2 + v2 * *deltaTime;
							double m2 = c2->mass;

							if (!(c2->ignoreLayers & c->layer) && !(c->ignoreLayers & c2->layer))
							{
								if (reg->AABBColliders.count(e2))
								{
									AABBCollider_Component* ac = &reg->AABBColliders[e2];

									if (ColliderFunctions::CircleWithRectangleIntersection(upcomingX1, cc->radius, upcomingX2, ac->width, ac->height))
									{
										ResolveCollision(c, c2, (upcomingX2 + Vector2D(ac->width / 2, ac->height / 2) - upcomingX1).normalize(), deltaTime);
									}
								}
							}
						}
					}
				}
				if (reg->AABBColliders.count(e))
				{
					AABBCollider_Component* ac = &reg->AABBColliders[e];
					Vector2D positionWithOffset = upcomingX1 + Vector2D(ac->offsetX, ac->offsetY);
					for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
					{
						if (reg->lineColliders.count(e2))
						{
							LineCollider_Component* cl = &reg->lineColliders[e2];

							Vector2D intersectionPoint, fromIntersectedLine, toIntersectedLine;
							double outA;
							if (ColliderFunctions::RectangleWithLineIntersection(ac->width, ac->height, positionWithOffset, cl->a, cl->b, &intersectionPoint, &fromIntersectedLine, &toIntersectedLine, &outA))
							{
								//!NOT WORKING AS INTENDET. IF NEEDED FIX SOON!!!
								Vector2D normal = ColliderFunctions::ReflectionNormal(cl, positionWithOffset + Vector2D(ac->width / 2, ac->height / 2));
								Vector2D response = ReflectionResponse(&normal, &v1);
								float l = response.length();
								Vector2D elasticityResult = (normal * l * (-c->elasticity + 1));
								Vector2D frictionResult = (response - normal * l) * c->friction;

								*c->position = ColliderFunctions::PositionToReturnToAfterCollision(fromIntersectedLine, toIntersectedLine, x1, outA, &intersectionPoint);

								c->velocity = response - elasticityResult - frictionResult;
							
							}
						}
					}
					for (int e2 = e + 1; e2 <= EntityManager::Instance()->num_entities; e2++)
					{
						if (reg->rigidbodies.count(e2))
						{
							Rigidbody_Component* c2 = &reg->rigidbodies[e2];

							//Basic rigidbody properties again for pretty look
							Vector2D v2 = c2->velocity;
							Vector2D x2 = *c2->position;
							Vector2D upcomingX2 = x2 + v2 * *deltaTime;
							
							double m2 = c2->mass;
							if (!(c2->ignoreLayers & c->layer) && !(c->ignoreLayers & c2->layer))
							{
								if (reg->AABBColliders.count(e2))
								{
									AABBCollider_Component* ac2 = &reg->AABBColliders[e2];
									Vector2D positionWithOffset2 = upcomingX2 + Vector2D(ac2->offsetX, ac2->offsetY);
									Vector2D normal;
									double penetration;
									if (ColliderFunctions::RectangleWithRectangleIntersection(ac->width, ac->height, positionWithOffset, ac2->width, ac2->height, positionWithOffset2, &normal, &penetration))
									{
										ResolveCollision(c, c2, normal, deltaTime);
										PositionalCorrection(c, c2, penetration, normal);
										x1 = *c->position;
										upcomingX1 = x1 + v1 * *deltaTime;
										positionWithOffset = upcomingX1 + Vector2D(ac->offsetX, ac->offsetY);
										
									}	
								}
							}
						}
						
					}
				}

				std::cout << c->velocity << std::endl;
				*c->position = *c->position + c->velocity * *deltaTime;
			}
		}
	}
}
#pragma endregion