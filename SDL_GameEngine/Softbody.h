#pragma once
#include "Vector2D.h"
#include <vector>
#include "SDL.h"
#include "Layers.h"
namespace Engine
{
	struct MassPoint
	{
		Vector2D position;
		double mass = 1;
		double gravityMultiplayer = 1;
		bool isStatic = false;

		void Lock() { isStatic = true; }
		void Unlock() { isStatic = false; }

		Vector2D velocity;
		Vector2D force;
		void ApplyForce(Vector2D force);
	};

	struct Spring
	{
		int A, B; // indexes of masspoints or framePoints
		double stiffness;
		double restLength;
		double dampingFactor;

		Vector2D springVelA, springVelB;
#ifdef _DEBUG
		double f;
#endif // _DEBUG
	};

	struct Softbody_Component
	{
		int massPointsN;
		std::vector<MassPoint> massPoints;
		std::vector<Spring> springs;
		double* gravity;
		double* dragCoeficient;
		double defaultStiffness;
		double defaultDampingFactor;
		bool rotateHardFrame = true;
		bool hardShapeMatching = true;
		bool gasShapeMatching = true;
		double shapeMatchingStiffness;
		double shapeMatchingDampingFactor;
		double shapeMatchingGasAmount;
		double outlineThickness = 0;
		double friction;
		double bounciness;
		Layers layer;
		Layers ignoreLayers;
		SDL_Color color;

		std::vector<Vector2D> closestPoints;
		std::vector<Vector2D> originalPositionsOfMassPoints;
		std::vector<Spring> springsForFrame;
		std::vector<Vector2D> finalPositionsOfHardFrame;
		Vector2D averagePosition;
		double lastFrameAverageAngle = 0;
		Sint16* x, * xFrame;
		Sint16* y, * yFrame;
	};

	class Softbody_System
	{
	public:
		void Init(class Registry* reg, const class Game* game);
		void StartUpdate(class Registry* reg);
		void Update(class Registry* reg, double* deltaTime, class Game* game);
		void CalculateSpringForce(Spring* s, Softbody_Component* c, Vector2D* forceA, Vector2D* forceB, double* deltaTime);
		void CalculateSpringForceForFrame(Spring* s, Softbody_Component* c, Vector2D* forceA, double* deltaTime, Vector2D framePosition);
		void ResolveCollision(MassPoint* A, MassPoint* B, MassPoint* P, Vector2D normal);
		void Draw(class Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect, const Game* game);
	};
}