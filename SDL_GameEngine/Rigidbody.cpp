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
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->rigidbodies.count(e))
		{
			Rigidbody_Component* c = &reg->rigidbodies[e];

			if (reg->circleCollider.count(e))
			{
				CircleCollider_Component* cc = &reg->circleCollider[e];
				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->lineCollider.count(e2))
					{
						LineCollider_Component* cl = &reg->lineCollider[e2];
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
						SDL_RenderDrawLine(renderer, lastPos.x - cc->radius * cl->normal.x, lastPos.y - cc->radius * cl->normal.y, lastPos.x + cc->radius * cl->normal.x, lastPos.y + cc->radius * cl->normal.y);
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
					}

				}
			}
			
		}
	}
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

			c->velocity = c->velocity + c->accelaration * *deltaTime;

			if (reg->circleCollider.count(e))
			{
				CircleCollider_Component* cc = &reg->circleCollider[e];

				Vector2D v1 = c->velocity;
				Vector2D x1 = *c->position;
				Vector2D upcomingX1 = *c->position + c->velocity * *deltaTime;
				float m1 = c->mass;
				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->lineCollider.count(e2))
					{
						LineCollider_Component* cl = &reg->lineCollider[e2];
						Vector2D pointOfIntersection;
						bool intersects = ColliderFunctions::FrameIndependentCircleWithLineIntersection(cl, cc, v1 * *deltaTime, &pointOfIntersection);
						if (intersects)
						{
							std::cout << "Collided" << std::endl;
							Vector2D normal = ColliderFunctions::ReflectionNormal(cl, *cc->position);
							c->velocity = ReflectionResponse(&normal, &v1);
							Vector2D add = ColliderFunctions::PositionToReturnToAfterCollision(&normal, cc, pointOfIntersection);
							*c->position = add;
						}
					}
				}
				for (int e2 = e + 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->rigidbodies.count(e2) && reg->circleCollider.count(e2) && e != e2)
					{
						Rigidbody_Component* c2 = &reg->rigidbodies[e2];
						CircleCollider_Component* cc2 = &reg->circleCollider[e2];


						Vector2D v2 = c2->velocity;
						Vector2D x2 = *c2->position;
						Vector2D upcomingX2 = *c2->position + c2->velocity * *deltaTime;
						float m2 = c2->mass;

						if (ColliderFunctions::CircleWithCircleIntersection(x2, cc2->radius, x1, cc->radius))
						{
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
			
			*c->position = *c->position + c->velocity * *deltaTime;
			lastPos = *c->position + c->velocity * *deltaTime;
		}
	}
}
