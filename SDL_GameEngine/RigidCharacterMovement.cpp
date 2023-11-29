#include "RigidCharacterMovement.h"
#include "Registry.h"
#include "Entity.h"
#include "Transform.h"
#include "InputManager.h"
#include "Rigidbody.h"
#include <algorithm>

namespace Engine
{
	void RigidCharacterMovement_System::Init(Registry* reg)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->rigidCharacterMovements.count(e) && reg->transforms.count(e) && reg->rigidbodies.count(e))
			{
				RigidCharacterMovement_Component* c = &reg->rigidCharacterMovements[e];
				c->transform = &reg->transforms[e];
				c->rigidbody = &reg->rigidbodies[e];
			}
		}
	}

	void RigidCharacterMovement_System::Update(Registry* reg, double* deltaTime)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->rigidCharacterMovements.count(e) && reg->transforms.count(e) && reg->rigidbodies.count(e))
			{
				RigidCharacterMovement_Component* c = &reg->rigidCharacterMovements[e];
				if (InputManager::Instance()->KeyPressed(c->moveRight))
				{
					c->holdTime = 0;
				}
				else if (InputManager::Instance()->KeyPressed(c->moveLeft))
				{
					c->holdTime = 0;
				}
				else if (InputManager::Instance()->KeyDown(c->moveRight))
				{
					c->holdTime += *deltaTime;
					double per = std::clamp<double>(c->holdTime / c->acceleration, 0, 1);
					reg->rigidbodies[e].velocity = Vector2D(c->moveSpeed * per, reg->rigidbodies[e].velocity.y);
				}
				else if (InputManager::Instance()->KeyDown(c->moveLeft))
				{
					c->holdTime += *deltaTime;
					double per = std::clamp<double>(c->holdTime / c->acceleration, 0, 1);
					reg->rigidbodies[e].velocity = Vector2D(-c->moveSpeed * per, reg->rigidbodies[e].velocity.y);
				}
				else if (c->holdTime > 0)
				{
					c->holdTime -= *deltaTime;
					double per = std::clamp<double>(c->holdTime / c->deceleration, 0, 1);
					reg->rigidbodies[e].velocity = Vector2D((reg->rigidbodies[e].velocity.x > 0 ? c->moveSpeed : -c->moveSpeed) * per, reg->rigidbodies[e].velocity.y);
				}

				if (InputManager::Instance()->KeyReleased(c->moveRight))
				{
					c->holdTime = std::min<float>({c->deceleration, c->holdTime});
				}
				else if (InputManager::Instance()->KeyReleased(c->moveLeft))
				{
					c->holdTime = std::min<float>({ c->deceleration, c->holdTime });
				}

				
			}
		}
	}
}