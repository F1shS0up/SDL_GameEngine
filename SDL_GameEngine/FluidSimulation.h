#pragma once
#include "Vector2D.h"
#include <vector>
#include "SDL.h"

struct Fluid_Component
{
	Vector2D startPos;
	Vector2D startAcc;
	SDL_Rect boundingBox;

	int particleCount;
	float startSpacing;
	float particleRadius;
	float targetDensity;
	float pressureMultiplier;

	std::vector<Vector2D> positions, velocities, predictedPositions;
	std::vector<float> densities;
};
class FluidSimulation_System
{
	//only one fluid simulation is possible
	Fluid_Component* fluid;

public:
	void Init(class Registry* reg);
	void Update(double* deltaTime);
	void ResolveCollision(int i);
	float CalculateDensity(Vector2D samplePoint);
	Vector2D CalculatePressureForce(int index);
	float ConvertDensityToPressure(float density);
	void Draw(SDL_Renderer* renderer);
};
