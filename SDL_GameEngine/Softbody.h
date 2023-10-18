#pragma once
#include "Vector2D.h"
#include <vector>
#include "SDL.h"

struct MassPoint
{
	Vector2D position;
	float mass;

	Vector2D velocity;
	Vector2D force;
};

struct Softbody_Component
{
	std::vector<MassPoint> massPoints;
	float* gravity;
};

class Softbody_System
{
public:
	void Init(class Registry* reg);
	void Update(class Registry* reg, double* deltaTime);
	void Draw(class Registry* reg, SDL_Renderer* renderer);
};
