#pragma once
#include "SDL.h"

namespace Engine
{
	struct RigidCharacterMovement_Component
	{
		float moveSpeed;
		float acceleration; //How many seconds does it take to get to full speed
		float deceleration; //How many seconds does it take to stop


		SDL_Scancode moveLeft, moveRight;

		SDL_Scancode jump;
		bool holdJump;
		float jumpStrength;

		class Transform_Component* transform;
		class Rigidbody_Component* rigidbody;
		float holdTime;
	};

	class RigidCharacterMovement_System
	{
	public:
		void Init(class Registry* reg);
		void Update(class Registry* reg, double* deltaTime);
	};
}
