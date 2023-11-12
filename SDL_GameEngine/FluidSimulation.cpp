#include "FluidSimulation.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include <math.h>
#include <algorithm>
#include <random>
#include "SDL2_gfxPrimitives.h"

namespace Engine
{

	const double smoothingRadius = 200;
	const double mass = 1;

	double SmoothingKernel(double radius, double dst)
	{
		if (dst >= radius) return 0;

		double volume = (M_PI * pow(radius, 4)) / 6;
		return (radius - dst) * (radius - dst) / volume;
	}
	double SmoothingKernelDerivative(double dst, double radius)
	{
		if (dst >= radius) return 0;

		double scale = 12 / (M_PI * std::pow(radius, 4));
		return (dst - radius) * scale;
	}

	void FluidSimulation_System::Init(Registry* reg)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->fluids.count(e))
			{
				Fluid_Component* fluid = &reg->fluids[e];
				int particlesPerRow = (int)std::sqrt(fluid->particleCount);
				int particlesPerCol = (fluid->particleCount - 1) / particlesPerRow + 1;
				double spacing = fluid->particleRadius * 2 + fluid->startSpacing;

				std::default_random_engine generator;
				std::uniform_int_distribution<int> distribution(960, 2900);
				std::uniform_int_distribution<int> distribution2(120, 2000);
				for (int i = 0; i < reg->fluids[e].particleCount; i++)
				{
					double x = (i % particlesPerRow - particlesPerRow / 2.f + 0.5f) * spacing;
					double y = (i / particlesPerRow - particlesPerCol / 2.f + 0.5f) * spacing;

					std::cout << "REACHED";
					fluid->positions.push_back(Vector2D(x + fluid->startPos.x, y + fluid->startPos.y));
					fluid->predictedPositions.push_back(Vector2D(x + fluid->startPos.x, y + fluid->startPos.y));
					fluid->velocities.push_back(Vector2D(0, 0));
					fluid->densities.push_back(fluid->targetDensity);
				}
				this->fluid = fluid;
			}
		}
	}

	void FluidSimulation_System::Update(double* deltaTime)
	{
		if (fluid)
		{
			for (int i = 0; i < fluid->particleCount; i++)
			{
				fluid->velocities[i] += fluid->startAcc * *deltaTime;
				fluid->predictedPositions[i] = fluid->positions[i] + fluid->velocities[i] * *deltaTime;
				fluid->densities[i] = CalculateDensity(fluid->predictedPositions[i]);

				Vector2D pressureForce = CalculatePressureForce(i);
				Vector2D pressureAcc = pressureForce / fluid->densities[i];
				fluid->velocities[i] += pressureAcc * *deltaTime;
				std::cout << fluid->velocities[i].x << std::endl;

				fluid->positions[i] += fluid->velocities[i] * *deltaTime;

				ResolveCollision(i);
			}
		}
	}

	void FluidSimulation_System::ResolveCollision(int i)
	{
		double down = fluid->positions[i].y + fluid->particleRadius;
		double up = fluid->positions[i].y - fluid->particleRadius;
		double right = fluid->positions[i].x + fluid->particleRadius;
		double left = fluid->positions[i].x - fluid->particleRadius;

		if (down > fluid->boundingBox.y + fluid->boundingBox.h)
		{
			fluid->velocities[i] = Vector2D(fluid->velocities[i].x, fluid->velocities[i].y * -0.1f);
			fluid->positions[i].y -= down - (fluid->boundingBox.y + fluid->boundingBox.h);
		}
		else if (up < fluid->boundingBox.y)
		{
			fluid->velocities[i] = Vector2D(fluid->velocities[i].x, fluid->velocities[i].y * -.1f);
			fluid->positions[i].y -= up - fluid->boundingBox.y;
		}
		if (right > fluid->boundingBox.x + fluid->boundingBox.w)
		{
			fluid->velocities[i] = Vector2D(fluid->velocities[i].x * -.1f, fluid->velocities[i].y);
			fluid->positions[i].x -= right - (fluid->boundingBox.x + fluid->boundingBox.w);
		}
		else if (left < fluid->boundingBox.x)
		{
			fluid->velocities[i] = Vector2D(fluid->velocities[i].x * -.1f, fluid->velocities[i].y);
			fluid->positions[i].x -= left - fluid->boundingBox.x;
		}
	}

	double FluidSimulation_System::CalculateDensity(Vector2D samplePoint)
	{
		double density = 0;

		for (Vector2D position : fluid->positions)
		{
			double dst = (position - samplePoint).length();
			double influence = SmoothingKernel(smoothingRadius, dst);
			density += mass * influence;
		}

		return density;
	}

	Vector2D FluidSimulation_System::CalculatePressureForce(int index)
	{
		Vector2D pressureForce = Vector2D(0, 0);

		for (int i = 0; i < fluid->particleCount; i++)
		{
			if (index == i) continue;

			Vector2D offset = fluid->positions[i] - fluid->positions[index];
			double dst = offset.length();
			Vector2D dir = dst == 0 ? Vector2D(1, 1) : offset / dst;

			double slope = SmoothingKernelDerivative(dst, smoothingRadius);
			double density = fluid->densities[i];
			double sharedPressure = (ConvertDensityToPressure(density) + ConvertDensityToPressure(fluid->densities[index])) / 2;
			pressureForce += dir * -sharedPressure * slope * mass / density;
		}

		return pressureForce;
	}
	double FluidSimulation_System::ConvertDensityToPressure(double density)
	{
		double densityError = density - fluid->targetDensity;
		double pressure = densityError * fluid->pressureMultiplier;
		return pressure;
	}

	void FluidSimulation_System::Draw(SDL_Renderer* renderer, const SDL_Rect* cameraRect)
	{
		if (fluid)
		{
			for (int i = 0; i < fluid->particleCount; i++)
			{
				filledCircleRGBA(renderer, fluid->positions[i].x - cameraRect->x, fluid->positions[i].y - cameraRect->y, fluid->particleRadius, 255, 255, 255, 1);
			}
		}
	}
}