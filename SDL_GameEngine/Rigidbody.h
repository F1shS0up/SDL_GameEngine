#pragma once
#include "Vector2D.h"
#include "SDL.h"
#include "Layers.h"
namespace Engine
{
	struct Rigidbody_Component
	{
		Vector2D velocity = Vector2D(0, 0);
		double* gravity;
		double mass = 10.f;
		double elasticity = 1;
		double friction = 1;
		Layers layer = A;
		Layers ignoreLayers;
		bool isStatic = false;

		Vector2D* position;
		double inverseMass;

		void ApplyForce(Vector2D force);
	};

	class Rigidbody_System
	{
	public:
		void Init(class Registry* reg);

		void Draw(class Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect);


		Vector2D ReflectionResponse(Vector2D* normal, Vector2D* velocity);
		void PositionalCorrection(Rigidbody_Component* A, Rigidbody_Component* B, double penetration, Vector2D collisionNormal);
		void ResolveCollision(Rigidbody_Component* A, Rigidbody_Component* B, Vector2D normal, double* deltaTime);

		void Update(class Registry* reg, double* deltaTime);
	};
}