#pragma once
#include "SDL.h"
#include <vector>
#include "Vector2D.h"

struct SoftbodyRotatingMovement_Component
{
	class Softbody_Component* softbody;
	double* force;
	std::vector<Vector2D> directionOfForce;
};
class SoftbodyRotatingMovement_System
{
public:
	void Init(class Registry* reg);
	void UpdateDirection(SoftbodyRotatingMovement_Component* movement, int rot);
	void Update(class Registry* reg, double* deltaTime);
};