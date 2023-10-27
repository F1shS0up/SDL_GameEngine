#include "SoftbodyRotatorMovement.h"
#include "Registry.h"
#include "Entity.h"
#include "InputManager.h"

void SoftbodyRotatingMovement_System::Init(Registry* reg)
{
	for (int e = 1; e < EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodyRotatingMovements.count(e))
		{
			UpdateDirection(&reg->softbodyRotatingMovements[e], 1);
		}
	}
}

void SoftbodyRotatingMovement_System::UpdateDirection(SoftbodyRotatingMovement_Component* movement, int rot)
{
	movement->directionOfForce.clear();
	for (int i = 0; i < movement->softbody->massPointsN; i++)
	{
		int nextIndex = 0;
		if (rot == 1)
		{
			if (i == movement->softbody->massPoints.size() - 1)
			{
				nextIndex = 0;
			}
			else
			{
				nextIndex = i + 1;
			}
		}
		else if (rot == -1)
		{
			if (i == 0)
			{
				nextIndex = movement->softbody->massPoints.size() - 1;
			}
			else
			{
				nextIndex = i - 1;
			}
		}
		else
		{
			return;
		}

		Vector2D dir = movement->softbody->massPoints[nextIndex].position - movement->softbody->massPoints[i].position;
		dir.normalize();

		
		movement->directionOfForce.push_back(dir);
	}
}

void SoftbodyRotatingMovement_System::Update(Registry* reg, double* deltaTime)
{
	for (int e = 1; e < EntityManager::Instance()->num_entities; e++)
	{
		if (reg->softbodyRotatingMovements.count(e))
		{
			SoftbodyRotatingMovement_Component* c = &reg->softbodyRotatingMovements[e];
			if (InputManager::Instance()->KeyDown(SDL_SCANCODE_RIGHT))
			{
				UpdateDirection(c, 1);
				for (int i = 0; i < c->softbody->massPointsN; i++)
				{
					MassPoint* m = &c->softbody->massPoints[i];
					m->ApplyForce(c->directionOfForce[i] * *c->force);
				}
			}
			else if (InputManager::Instance()->KeyDown(SDL_SCANCODE_LEFT))
			{
				UpdateDirection(c, -1);
				for (int i = 0; i < c->softbody->massPointsN; i++)
				{
					MassPoint* m = &c->softbody->massPoints[i];
					m->ApplyForce(c->directionOfForce[i] * *c->force);
				}
			}
		}
	}
}
