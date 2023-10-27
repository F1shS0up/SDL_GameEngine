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
	double startSpacing;
	double particleRadius;
	double targetDensity;
	double pressureMultiplier;

	std::vector<Vector2D> positions, velocities, predictedPositions;
	std::vector<double> densities;
};
class FluidSimulation_System
{
	//only one fluid simulation is possible
	Fluid_Component* fluid;

public:
	void Init(class Registry* reg);
	void Update(double* deltaTime);
	void ResolveCollision(int i);
	double CalculateDensity(Vector2D samplePoint);
	Vector2D CalculatePressureForce(int index);
	double ConvertDensityToPressure(double density);
	void Draw(SDL_Renderer* renderer);
};
