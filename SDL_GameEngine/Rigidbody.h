#pragma once
#include "Vector2D.h"
#include "SDL.h"

struct Rigidbody_Component
{
	Vector2D velocity = Vector2D(0, 0);
	Vector2D accelaration = Vector2D(0, 0);
	float mass = 10.f;
	float elasticity = 1;
	bool isStatic = false;

	Vector2D* position;

	void ApplyForce(Vector2D force);
};

class Rigidbody_System
{
public:
	void Init(class Registry* reg);
#ifdef  _DEBUG
	void Draw(class Registry* reg, SDL_Renderer* renderer);
#endif //  _DEBUG

	Vector2D ReflectionResponse(Vector2D* normal, Vector2D* velocity);
	void ResolveCollision(Rigidbody_Component* A, Rigidbody_Component* B);
	void CorrectPosition(Rigidbody_Component* A, Rigidbody_Component* B, float* penetration);

	void Update(class Registry* reg, double* deltaTime);
};