#pragma once
#include "Vector2D.h"
#include "SDL.h"

enum Layers
{
	A = 1,   // 0x01
	B = 2,   // 0x02
	C = 3,   // 0x04
	D = 4,   // 0x08
	E = 5,   // 0x10
	F = 6   // 0x20A = 1<<0
};

inline Layers operator|(Layers a, Layers b)
{
	return static_cast<Layers>(static_cast<int>(a) | static_cast<int>(b));
}

struct Rigidbody_Component
{
	Vector2D velocity = Vector2D(0, 0);
	Vector2D accelaration = Vector2D(0, 0);
	float mass = 10.f;
	float elasticity = 1;
	Layers layer = A;
	Layers ignoreLayers;

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
	void ResolveCollision(Rigidbody_Component* A, Rigidbody_Component* B, Vector2D normal);
	void CorrectPosition(Rigidbody_Component* A, Rigidbody_Component* B, float* penetration);

	void Update(class Registry* reg, double* deltaTime);
};