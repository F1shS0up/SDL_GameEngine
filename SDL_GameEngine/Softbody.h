#pragma once
#include "Vector2D.h"
#include <vector>
#include "SDL.h"

struct MassPoint
{
	Vector2D position;
	float mass;
	bool isStatic;

	Vector2D velocity;
	Vector2D force;
};

struct Spring
{
	int A, B; // indexes of masspoints
	float stiffness;
	float restLength;
	float dampingFactor;

	Vector2D springVelA, springVelB;
};

struct Softbody_Component
{
	std::vector<MassPoint> massPoints;
	std::vector<Spring> springs;
	float* gravity;
};

class Softbody_System
{
public:
	void Init(class Registry* reg);
	void Update(class Registry* reg, double* deltaTime);
	Vector2D CalculateSpringForce(Spring* s, Softbody_Component* c, Vector2D* forceA, Vector2D* forceB, double* deltaTime);
	void Draw(class Registry* reg, SDL_Renderer* renderer);
};
