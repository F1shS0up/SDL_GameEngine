#pragma once
#include "Vector2D.h"
#include <vector>
#include "SDL.h"

struct MassPoint
{
	Vector2D position;
	float mass;
	bool isStatic;
	void Lock() { isStatic = true; }
	void Unlock() { isStatic = false; }

	Vector2D velocity;
	Vector2D force;
	void ApplyForce(Vector2D force);

};

struct Spring
{
	int A, B; // indexes of masspoints or framePoints
	float stiffness;
	float restLength;
	float dampingFactor;

	Vector2D springVelA, springVelB;
#ifdef _DEBUG
	float f;
#endif // _DEBUG
};

struct Softbody_Component
{
	int massPointsN;
	std::vector<MassPoint> massPoints;
	std::vector<Spring> springs;
	float* gravity;
	float* dragCoeficient;
	float defaultStiffness;
	float defaultDampingFactor;
	bool hardShapeMatching = true;

	std::vector<Vector2D> closestPoints;
	std::vector<Vector2D> originalPositionsOfMassPoints;
	std::vector<Spring> springsForFrame;
	std::vector<Vector2D> finalPositionsOfHardFrame;
	float lastFrameAngle = 0;
	Sint16* x,* xFrame;
	Sint16* y,* yFrame;
};


class Softbody_System
{
public:
	void Init(class Registry* reg);
	void Update(class Registry* reg, double* deltaTime);
	void CalculateSpringForce(Spring* s, Softbody_Component* c, Vector2D* forceA, Vector2D* forceB, double* deltaTime);
	void CalculateSpringForceForFrame(Spring* s, Softbody_Component* c, Vector2D* forceA, double* deltaTime, Vector2D framePosition);
	void ResolveCollision(MassPoint* A, MassPoint* B, MassPoint* P, Vector2D normal);
	void Draw(class Registry* reg, SDL_Renderer* renderer);
};
