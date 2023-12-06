#pragma once
#include "SDL.h"

namespace Engine
{
	struct RigidCharacterMovement_Component
	{
		const char* idleAnim;
		const char* moveRightAnim[4]; //Each element means 25% of speed
		const char* moveLeftAnim[4]; //Each element means 25% of speed

#pragma region Moving

		float moveSpeed;
		float acceleration; //How many seconds does it take to get to full speed
		float deceleration; //How many seconds does it take to stop
		SDL_Scancode moveLeft, moveRight;
#pragma endregion
#pragma region Jump

		bool holdJump;
		float jumpStrength;
		SDL_Scancode jump;
#pragma endregion




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
