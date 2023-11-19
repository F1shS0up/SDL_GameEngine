#pragma once
#include "Vector2D.h"
#include "SDL.h"
#include <array>

namespace Engine
{
	struct CircleCollider_Component
	{
		double radius;

		Vector2D* position;
	};
	struct LineCollider_Component
	{
		Vector2D a, b;
		Vector2D mid;
		Vector2D normal;
	};

	struct AABBCollider_Component
	{
		double width, height;

		Vector2D* position;
	};

	class Collider_System
	{
	public:

		void Init(class Registry* reg);
		void Draw(class Registry* reg, SDL_Renderer* renderer, const SDL_Rect* cameraRect);
	};

	namespace ColliderFunctions
	{
		//Check collision between two lines and return true if they collide/intersect
		bool LineLineIntersection(const Vector2D& A1, const Vector2D& A2, const Vector2D& B1, const Vector2D& B2, Vector2D* intersection = 0, double* outA = 0);

		bool LineIntersectsHorizontalInfiniteLine(Vector2D infLine, Vector2D A, Vector2D B);

		bool LineIntersectsHorizontalLine(const Vector2D* A, const Vector2D* B, const Vector2D* A2, const Vector2D* B2);

		Vector2D ClosestPointToLine(const Vector2D* A, const Vector2D* B, const Vector2D* P);

		//Calculates the furthest and nerest point in the circle and if it intersects with line.
		bool CircleWithLineIntersection(LineCollider_Component* lineColliders, Vector2D position, double radius, Vector2D* intersectionPoint);

		//Check if two circles intersect
		bool CircleWithCircleIntersection(Vector2D posA, double radiusA, Vector2D posB, double radiusB, double* penetration = 0);

		//Check if rectangle intersects line, only works with non rotated rectangles
		bool RectangleWithLineIntersection(double width, double height, Vector2D pos, Vector2D A, Vector2D B, Vector2D* intersection = 0, Vector2D* from = 0, Vector2D* to = 0, double* outA = 0);

		bool RectangleWithRectangleIntersection(double widthA, double heightA, Vector2D posA, double widthB, double heightB, Vector2D posB, Vector2D* normal = 0, double* penetration = 0);

		//It does the same except but if it fails it checks from middle to middle
		bool FrameIndependentCircleWithLineIntersection(LineCollider_Component* lineColliders, const CircleCollider_Component* circleColliders, Vector2D nextFramePosToAdd, Vector2D* intersectionPoint);

		bool CircleWithRectangleIntersection(Vector2D pos, double radius, Vector2D pos2, double width, double height);

		//Return the normal of line thats facing towards the point
		Vector2D ReflectionNormal(const LineCollider_Component* lineColliders, Vector2D point);

		Vector2D ReflectionNormal(Vector2D A, Vector2D B, Vector2D point);

		//Position to go to when collision was detected for circle with line
		Vector2D PositionToReturnToAfterCollision(const Vector2D* normal, const CircleCollider_Component* circleColliders, Vector2D intersection);

		//Position to go to when collision was detected for circle with line but this time between frames which is useful when low frame rates are expected
		Vector2D PositionToReturnToAfterCollision(const Vector2D* normal, Vector2D pos, Vector2D newPos, double radius, Vector2D intersection, const LineCollider_Component* lc);

		//Position to go to when collision was detected for rectangle with line
		Vector2D PositionToReturnToAfterCollision(Vector2D from, Vector2D to, Vector2D pos, double percentage, const Vector2D* intersectionPoint);
	}
}