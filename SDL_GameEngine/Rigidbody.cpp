#include "Rigidbody.h"
#include "Entity.h"
#include "Registry.h"

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
				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->lineCollider.count(e2))
					{
						LineCollider_Component* cl = &reg->lineCollider[e2];
						Vector2D pointOfIntersection;
						bool intersects = ColliderFunctions::CircleWithLineIntersection(cl, *cc->position + c->velocity * *deltaTime, cc->radius, &pointOfIntersection);
						if (intersects)
						{
							std::cout << "Collided" << std::endl;
							Vector2D normal = ColliderFunctions::ReflectionNormal(cl, *cc->position);
							c->velocity = ColliderFunctions::ReflectionResponse(&normal, &c->velocity);
							Vector2D add = ColliderFunctions::PositionToReturnToAfterCollision(&normal, cc, pointOfIntersection);
							*c->position = add;
							return;

						}
					}

				}
			}
			
			*c->position = *c->position + c->velocity * *deltaTime;
			lastPos = *c->position + c->velocity * *deltaTime;
		}
	}
}
