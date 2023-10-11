#include "Rigidbody.h"
#include "Entity.h"
#include "Registry.h"
#include <cmath>
#include <numeric>

#define ComputeVelocity(v1, v2, x1, x2, m1, m2, out)\
 *out = v1 - Vector2D((2 * m2 / (m1 + m2)) * VectorDotProduct(v1 - v2, x1 - x2) / ((x1 - x2).length() * (x1 - x2).length())* (x1 - x2).x, (2 * m2 / (m1 + m2)) * VectorDotProduct(v1 - v2, x1 - x2) / ((x1 - x2).length() * (x1 - x2).length())* (x1 - x2).y)\


float VectorDotProduct(Vector2D v1, Vector2D v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

void Rigidbody_System::Init(Registry* reg)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->rigidbodies.count(e))
		{
			Rigidbody_Component* c = &reg->rigidbodies[e];

			if (reg->transforms.count(e))
			{
				Transform_Component* cT = &reg->transforms[e];
				c->position = &cT->position;
			}
		}
	}
}
#ifdef _DEBUG
void Rigidbody_System::Draw(Registry* reg, SDL_Renderer* renderer)
{
	
}
#endif

Vector2D Rigidbody_System::ReflectionResponse(Vector2D* normal, Vector2D* velocity)
{
	// u = v * n * n * n
	float dot = VectorDotProduct(*velocity, *normal);

	Vector2D u = Vector2D(dot * normal->x, dot * normal->y);
	Vector2D w = *velocity - u;
	// w = v - u
	// new v = w - u

	// (v - v * n * n * n) - v * n * n * n

	Vector2D result = w - u;

	return result;
}

void Rigidbody_System::Update(Registry* reg, double* deltaTime)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->rigidbodies.count(e))
		{
			Rigidbody_Component* c = &reg->rigidbodies[e];

			//Rigidbody properties to make it look better
			Vector2D v1 = c->velocity;
			Vector2D x1 = *c->position;
			float m1 = c->mass;


			//Update the basic velocity, acceleration is usually gravity
			c->velocity = c->velocity + c->accelaration * *deltaTime;


			if (reg->circleColliders.count(e))
			{
				CircleCollider_Component* cc = &reg->circleColliders[e];
				
				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->lineColliders.count(e2))
					{
						LineCollider_Component* cl = &reg->lineColliders[e2];

						Vector2D pointOfIntersection;
						bool intersects = ColliderFunctions::FrameIndependentCircleWithLineIntersection(cl, cc, v1 * *deltaTime, &pointOfIntersection);
						if (intersects)
						{
							//Collided with line

							//Get the normal which is closer to the middle
							Vector2D normal = ColliderFunctions::ReflectionNormal(cl, *cc->position);
							//Calculate the response
							c->velocity = ReflectionResponse(&normal, &v1);
							//Make sure that the position is awway from line so that the collision doesn't happen twice
							*c->position = ColliderFunctions::PositionToReturnToAfterCollision(&normal, cc, pointOfIntersection);
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
						float m2 = c2->mass;

						if (reg->circleColliders.count(e2))
						{
							CircleCollider_Component* cc2 = &reg->circleColliders[e2];

							if (ColliderFunctions::CircleWithCircleIntersection(x2, cc2->radius, x1, cc->radius))
							{
								//Collided with another circle

								//Calculate the velocity using the K.E. formula and conservation of energy
								//Then save it in new variable, because we will still use the before collision velocity.
								Vector2D v1new;
								ComputeVelocity(v1, v2, x1, x2, m1, m2, &v1new);

								Vector2D v2new;
								ComputeVelocity(v2, v1, x2, x1, m2, m1, &v2new);

								c->velocity = v1new;
								c2->velocity = v2new;

							}
						}
					}
				}
			}
			if (reg->rectangleColliders.count(e))
			{
				RectangleCollider_Component* rc = &reg->rectangleColliders[e];

				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->lineColliders.count(e2))
					{
						LineCollider_Component* cl = &reg->lineColliders[e2];

						Vector2D intersectionPoint, fromIntersectedLine, toIntersectedLine;
						float outA;
						if (ColliderFunctions::RectangleWithLineIntersection(rc->width, rc->height, x1, cl->a, cl->b, &intersectionPoint, &fromIntersectedLine, &toIntersectedLine, &outA))
						{
							Vector2D normal = ColliderFunctions::ReflectionNormal(cl, x1);
							c->velocity = ReflectionResponse(&normal, &v1);

							//Set the position away from the line so that it wont collide again which could result in a rectangle being stuck in line
							*c->position = ColliderFunctions::PositionToReturnToAfterCollision(fromIntersectedLine, toIntersectedLine, x1, outA, &intersectionPoint);
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
						float m2 = c2->mass;

						if (reg->rectangleColliders.count(e2))
						{
							RectangleCollider_Component* rc2 = &reg->rectangleColliders[e2];
							if (ColliderFunctions::RectangleWithRectangleIntersection(rc->width, rc->height, x1, rc2->width, rc2->height, x2))
							{
								//Collided with another rectangle

								//Calculate the velocity using the K.E. formula and conservation of energy
								//Then save it in new variable, because we will still use the before collision velocity.
								Vector2D v1new;
								ComputeVelocity(v1, v2, x1, x2, m1, m2, &v1new);

								Vector2D v2new;
								ComputeVelocity(v2, v1, x2, x1, m2, m1, &v2new);

								c->velocity = v1new;
								c2->velocity = v2new;
							}
						}
					}
				}
			}
			
			*c->position = *c->position + c->velocity * *deltaTime;
		}
	}
}
