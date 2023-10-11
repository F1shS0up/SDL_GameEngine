#pragma once
#include "Vector2D.h"
#include "SDL.h"

struct Rigidbody_Component
{
	Vector2D velocity = Vector2D(0, 0);
	Vector2D accelaration = Vector2D(0, 0);
	float mass = 10.f;

	Vector2D* position;
};

class Rigidbody_System
{
public:
	void Init(class Registry* reg);
#ifdef  _DEBUG
	void Draw(class Registry* reg, SDL_Renderer* renderer);
#endif //  _DEBUG

	Vector2D ReflectionResponse(Vector2D* normal, Vector2D* velocity);

	void Update(class Registry* reg, double* deltaTime);



	Vector2D lastPos;
};