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

struct RectangleCollider_Component
{
	float width, height;

	Vector2D* position;
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
	//Check collision between two lines and return true if they collide/intersect
	bool LineCollision(const Vector2D& A1, const Vector2D& A2, const Vector2D& B1, const Vector2D& B2, Vector2D* intersection, float* outA = 0);

	//Calculates the furthest and nerest point in the circle and if it intersects with line.
	bool CircleWithLineIntersection(LineCollider_Component* lineColliders, Vector2D position, float radius, Vector2D* intersectionPoint);

	//It does the same except but if it fails it checks from middle to middle
	bool FrameIndependentCircleWithLineIntersection(LineCollider_Component* lineColliders, const CircleCollider_Component* circleColliders, Vector2D nextFramePosToAdd, Vector2D* intersectionPoint);

	//Check if two circles intersect
	bool CircleWithCircleIntersection(Vector2D posA, float radiusA, Vector2D posB, float radiusB);

	//Return the normal of line thats facing towards the point
	Vector2D ReflectionNormal(const LineCollider_Component* lineColliders, Vector2D point);

	//Position to go to when collision was detected for circle with line
	Vector2D PositionToReturnToAfterCollision(const Vector2D* normal, const CircleCollider_Component* circleColliders, Vector2D intersection);

	//Position to go to when collision was detected for circle with line but this time between frames which is useful when low frame rates are expected
	Vector2D PositionToReturnToAfterCollision(const Vector2D* normal, Vector2D pos, Vector2D newPos, float radius, Vector2D intersection, const LineCollider_Component* lc);

	//Position to go to when collision was detected for rectangle with line
	Vector2D PositionToReturnToAfterCollision(Vector2D from, Vector2D to, Vector2D pos, float percentage, const Vector2D* intersectionPoint);

	//Check if rectangle intersects line, only works with non rotated rectangles
	bool RectangleWithLineIntersection(float width, float height, Vector2D pos, Vector2D A, Vector2D B, Vector2D* intersection = 0, Vector2D* from = 0, Vector2D* to = 0, float* outA = 0);

	bool RectangleWithRectangleIntersection(float widthA, float heightA, Vector2D posA, float widthB, float heightB, Vector2D posB);

}