#pragma once
#include "Vector2D.h"
#include "SDL.h"
#include <array>

struct CircleCollider_Component
{
	float radius;

	Vector2D* position;
};
struct LineCollider_Component
{
	Vector2D a, b;
	Vector2D mid;
	Vector2D normal;
};

class Collider_System
{
public:

	void Init(class Registry* reg);
#ifdef _DEBUG
	void Draw(class Registry* reg, SDL_Renderer* renderer);
#endif // DEBUG

};

namespace ColliderFunctions
{

	static Vector2D vel, pos;

	bool LineCollision(const Vector2D& A1, const Vector2D& A2,
		const Vector2D& B1, const Vector2D& B2,
		Vector2D* intersection = 0);

	//Calculates the furthest and nerest point in the circle and if it intersects with line.
	bool CircleWithLineIntersection(LineCollider_Component* lineCollider, Vector2D position, float radius, Vector2D* intersectionPoint);

	//It does the same except but if it fails it checks from middle to middle
	bool FrameIndependentCircleWithLineIntersection(LineCollider_Component* lineCollider, CircleCollider_Component* circleCollider, Vector2D nextFramePosToAdd, Vector2D* intersectionPoint);
	bool CircleWithCircleIntersection(Vector2D posA, float radiusA, Vector2D posB, float radiusB);

	Vector2D ReflectionNormal(LineCollider_Component* lineCollider, Vector2D point);

	Vector2D PositionToReturnToAfterCollision(Vector2D* normal, CircleCollider_Component* circleCollider, Vector2D intersection);
	Vector2D PositionToReturnToAfterCollision(Vector2D* normal, Vector2D pos, Vector2D newPos, float radius, Vector2D intersection, LineCollider_Component lc);
	Vector2D PositionToReturnToAfterCollision(Vector2D pos, float radius, Vector2D pos2, float radius2);

}