#pragma once
#include "SDL.h"
#include <vector>
#include "Vector2D.h"

enum JointType
{
	StaticJoint, SpringJoint
};

struct SoftbodyJoint_Values
{
	JointType tp;
	class Softbody_Component* otherBody;

	std::vector<class MassPoint*> moveBodyAnchorPoints;
	std::vector<class MassPoint*> otherBodyAnchorPoints;
	Vector2D springVel = Vector2D(0, 0);
};

struct SoftbodyJoint_Component
{
	std::vector<SoftbodyJoint_Values> values;
};

class SoftbodyJoint_System
{
public:
	void Init(class Registry* reg);
	void Update(class Registry* reg, double* deltaTime);
	void CalculateSpringForce(Vector2D APosition, Vector2D BPosition, float stiffness, float restLength, float dampingFactor, Vector2D velocityA, Vector2D velocityB, Vector2D* forceA, Vector2D* forceB, double* deltaTime);
};