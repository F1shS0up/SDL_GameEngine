#include "Collider.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include <numeric>
#include <complex>
#include "SDL2_gfxPrimitives.h"
void Collider_System::Init(Registry* reg)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		//Loop through all collider types and assign some basic variables
		//Such as position, normal etc...

		if (reg->circleColliders.count(e))
		{
			CircleCollider_Component* c = &reg->circleColliders[e];

			if (reg->transforms.count(e))
			{
				Transform_Component* cT = &reg->transforms[e];
				c->position = &cT->position;
			}
		}
		if (reg->AABBColliders.count(e))
		{
			AABBCollider_Component* c = &reg->AABBColliders[e];

			if (reg->transforms.count(e))
			{
				Transform_Component* cT = &reg->transforms[e];
				c->position = &cT->position;
			}
		}
		if (reg->lineColliders.count(e))
		{
			LineCollider_Component* c = &reg->lineColliders[e];
			double da = c->b.x - c->a.x;
			double db = c->b.y - c->a.y;

			c->normal = Vector2D(-db, da).normalize();

			c->mid = Vector2D(c->a.x + (c->b.x - c->a.x) / 2, c->a.y + (c->b.y - c->a.y) / 2);
		}
	}
}

#ifdef _DEBUG
void Collider_System::Draw(Registry* reg, class SDL_Renderer* renderer, const SDL_Rect* cameraRect)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->lineColliders.count(e))
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
			SDL_RenderDrawLine(renderer, reg->lineColliders[e].a.x, reg->lineColliders[e].a.y, reg->lineColliders[e].b.x, reg->lineColliders[e].b.y);

			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 1);
			SDL_RenderDrawLine(renderer, reg->lineColliders[e].mid.x, reg->lineColliders[e].mid.y, reg->lineColliders[e].mid.x + reg->lineColliders[e].normal.x * 200, reg->lineColliders[e].mid.y + reg->lineColliders[e].normal.y * 200);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
			SDL_RenderDrawLine(renderer, reg->lineColliders[e].mid.x, reg->lineColliders[e].mid.y, reg->lineColliders[e].mid.x - reg->lineColliders[e].normal.x * 200, reg->lineColliders[e].mid.y - reg->lineColliders[e].normal.y * 200);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);

			for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
			{
				if (reg->circleColliders.count(e2))
				{
					LineCollider_Component* cl = &reg->lineColliders[e];
					CircleCollider_Component* cc = &reg->circleColliders[e2];

					Vector2D intersection;
					bool inter = ColliderFunctions::LineLineIntersection(*cc->position + cl->normal * cc->radius, *cc->position - cl->normal * cc->radius, cl->a, cl->b, &intersection);
					if (inter)
					{
						filledCircleRGBA(renderer, intersection.x, intersection.y, 20, 255, 255, 0, 1 );
					}
				}
			}
		}
	}
}
#endif // _DEBUG

//Reflect point around the A and B line
inline Vector2D reflect(Vector2D P, Vector2D A, Vector2D B)
{
	typedef std::complex<double> point;
	// Performing translation and shifting origin at A
	point Pt = point(P.x - A.x, P.y - A.y);
	point Bt = point(B.x - A.x, B.y - A.y);

	// Performing rotation in clockwise direction
	// BtAt becomes the X-Axis in the new coordinate system
	point Pr = Pt / Bt;

	// Reflection of Pr about the new X-Axis
	// Followed by restoring from rotation
	// Followed by restoring from translation
	point x = std::conj(Pr) * Bt + point(A.x, A.y);
	return Vector2D(x.real(), x.imag());
}

bool ColliderFunctions::LineLineIntersection(const Vector2D& A1, const Vector2D& A2, const Vector2D& B1, const Vector2D& B2, Vector2D* intersection, double* outA)
{
	Vector2D a(A2 - A1);
	Vector2D b(B2 - B1);

	double f = Vector2D::PerpDotProduct(a, b);
	if (!f)      // lines are parallel
		return false;

	Vector2D c(B2 - A2);
	double aa = Vector2D::PerpDotProduct(a, c);
	double bb = Vector2D::PerpDotProduct(b, c);

	if (f < 0)
	{
		if (aa > 0)     return false;
		if (bb > 0)     return false;
		if (aa < f)     return false;
		if (bb < f)     return false;
	}
	else
	{
		if (aa < 0)     return false;
		if (bb < 0)     return false;
		if (aa > f)     return false;
		if (bb > f)     return false;
	}

	if (intersection)
		*intersection = (B2 - B1) * (1.0 - (aa / f)) + B1;
	if (outA)
		*outA = aa;
	return true;
}
bool ColliderFunctions::LineIntersectsHorizontalInfiniteLine(Vector2D infLine, Vector2D A, Vector2D B)
{
	bool X = A.x > infLine.x || B.x > infLine.x;
	bool Y = (A.y > infLine.y && B.y < infLine.y) || (A.y < infLine.y && B.y > infLine.y);
	return X && Y;
}
bool ColliderFunctions::LineIntersectsHorizontalLine(const Vector2D* A, const Vector2D* B, const Vector2D* A2, const Vector2D* B2)
{
	return ((A->y > A2->y && B->y < A2->y) || (A->y < A2->y && B->y > A2->y)) && ((A->x > A2->x && A->x < B2->x) || (A->x < A2->x && A->x > B2->x) || (B->x > A2->x && B->x < B2->x) || (B->x < A2->x && B->x > B2->x));
}
Vector2D ColliderFunctions::ClosestPointToLine(const Vector2D* A, const Vector2D* B, const Vector2D* P)
{

	//Get heading
	Vector2D heading = (*B - *A);
	double magnitudeMax = heading.length();
	heading.normalize();

	//Do projection from the point but clamp it
	Vector2D lhs = *P - *A;
	double dotP = Vector2D::DotProduct(lhs, heading);
	dotP = std::clamp<double>(dotP, 0, magnitudeMax);
	return *A + heading * dotP;
}

bool ColliderFunctions::CircleWithLineIntersection(LineCollider_Component* lineColliders, Vector2D position, double radius, Vector2D* intersectionPoint)
{
	Vector2D a1, a2, b1, b2;
	a1 = lineColliders->a;
	b1 = lineColliders->b;

	a2 = position + lineColliders->normal * radius;
	// b2 is equal to the center of the circle and normal of line times radius
	b2 = position - lineColliders->normal * radius;

	Vector2D intersection;
	bool intersects = LineLineIntersection(a1, b1, a2, b2, &intersection);

	*intersectionPoint = intersection;

	return intersects;
}

bool ColliderFunctions::FrameIndependentCircleWithLineIntersection(LineCollider_Component* lineColliders, const CircleCollider_Component* circleColliders, Vector2D nextFramePosToAdd, Vector2D* intersectionPoint)
{
	bool intersect = CircleWithLineIntersection(lineColliders, *circleColliders->position + nextFramePosToAdd, circleColliders->radius, intersectionPoint);
	if (intersect)
	{
		return true;
	}

	Vector2D a1, a2, b1, b2;
	a1 = lineColliders->a;
	b1 = lineColliders->b;

	a2 = *circleColliders->position + nextFramePosToAdd;
	// b2 is equal to the center of the circle and normal of line times radius
	b2 = *circleColliders->position;
	intersect = LineLineIntersection(a1, b1, a2, b2, intersectionPoint);
	if (intersect)
	{
		return true;
	}

	return false;
}

bool ColliderFunctions::CircleWithCircleIntersection(Vector2D posA, double radiusA, Vector2D posB, double radiusB, double* penetration)
{
	Vector2D difference = Vector2D(std::abs(posB.x - posA.x), std::abs(posB.y - posA.y));
	double distance = difference.length();

	*penetration = radiusA + radiusB - distance;

	return distance < radiusA + radiusB;
}

bool ColliderFunctions::RectangleWithLineIntersection(double width, double height, Vector2D pos, Vector2D A, Vector2D B, Vector2D* intersection, Vector2D* from, Vector2D* to, double* outA)
{
	if (ColliderFunctions::LineLineIntersection(Vector2D(pos.x, pos.y), Vector2D(pos.x + width, pos.y), A, B, intersection, outA))
	{
		*from = pos;
		*to = Vector2D(pos.x + width, pos.y);
		return true;
	}
	if (ColliderFunctions::LineLineIntersection(Vector2D(pos.x + width, pos.y), Vector2D(pos.x + width, pos.y + height), A, B, intersection, outA))
	{
		*from = Vector2D(pos.x + width, pos.y);
		*to = Vector2D(pos.x + width, pos.y + height);
		return true;
	}
	if (ColliderFunctions::LineLineIntersection(Vector2D(pos.x + width, pos.y + height), Vector2D(pos.x, pos.y + height), A, B, intersection, outA))
	{
		*from = Vector2D(pos.x + width, pos.y + height);
		*to = Vector2D(pos.x, pos.y + height);
		return true;
	}
	if (ColliderFunctions::LineLineIntersection(Vector2D(pos.x, pos.y + height), Vector2D(pos.x, pos.y), A, B, intersection, outA))
	{
		*from = Vector2D(pos.x, pos.y + height);
		*to = pos;
		return true;
	}
	return false;
}

bool ColliderFunctions::RectangleWithRectangleIntersection(double widthA, double heightA, Vector2D posA, double widthB, double heightB, Vector2D posB, Vector2D* normal, double* penetration)
{
	SDL_Rect rA = { posA.x, posA.y, widthA, heightA };
	SDL_Rect rB = { posB.x, posB.y, widthB, heightB };
	SDL_Rect result;
	if (SDL_IntersectRect(&rA, &rB, &result))
	{
		if (normal)
		{
			if (result.h > result.w && posA.x + widthA > posB.x && posA.x + widthA < posB.x + widthB)
			{
				//B on right
				*normal = Vector2D(1, 0);
				if (penetration)*penetration = result.w;
			}
			if (result.h > result.w && posB.x + widthB > posA.x && posB.x + widthB < posA.x + widthA)
			{
				//B on left
				*normal = Vector2D(-1, 0);
				if (penetration)*penetration = result.w;
			}
			if (result.h < result.w && posA.y + heightA > posB.y && posA.y + heightA < posB.y + heightB)
			{
				//B on bottom
				*normal = Vector2D(0, 1);
				if(penetration)*penetration = result.h;
			}
			if (result.h < result.w && posB.y + heightB > posA.y && posB.y + heightB < posA.y + heightA)
			{
				//B on top
				*normal = Vector2D(0, -1);
				if (penetration)*penetration = result.h;
			}
		}

		return true;
	}

	return false;
}

bool ColliderFunctions::CircleWithRectangleIntersection(Vector2D pos, double radius, Vector2D pos2, double width, double height)
{
	Vector2D circleDistance;
	Vector2D rectMid = pos2 + Vector2D(width / 2, height / 2);
	circleDistance.x = std::abs(rectMid.x - pos.x);
	circleDistance.y = std::abs(rectMid.y - pos.y);

	if (circleDistance.x > (width / 2 + radius)) { return false; }
	if (circleDistance.y > (height / 2 + radius)) { return false; }

	if (circleDistance.x <= (width / 2)) { return true; }
	if (circleDistance.y <= (height / 2)) { return true; }

	double cornerDistance_sq;
	cornerDistance_sq = (circleDistance.x - width / 2) * (circleDistance.x - width / 2) +
		(circleDistance.y - height / 2) * (circleDistance.y - height / 2);

	return (cornerDistance_sq <= (radius * radius));
}

Vector2D ColliderFunctions::ReflectionNormal(const LineCollider_Component* lineColliders, Vector2D point)
{
	Vector2D normalA, normalB;

	normalA = lineColliders->mid + lineColliders->normal;
	normalB = lineColliders->mid - lineColliders->normal;

	double lengthA = (normalA - point).length();
	double lengthB = (normalB - point).length();

	return (lengthA < lengthB) ? lineColliders->normal : Vector2D(-lineColliders->normal.x, -lineColliders->normal.y);
}

Vector2D ColliderFunctions::PositionToReturnToAfterCollision(const Vector2D* normal, const CircleCollider_Component* circleColliders, Vector2D intersection)
{
	Vector2D pos = *circleColliders->position;
	Vector2D biggestPos = pos - Vector2D(circleColliders->radius * normal->x, circleColliders->radius * normal->y);
	Vector2D result = (biggestPos - intersection) * -1;
	return result + pos;
}

Vector2D ColliderFunctions::PositionToReturnToAfterCollision(const Vector2D* normal, Vector2D pos, Vector2D newPos, double radius, Vector2D intersection, const LineCollider_Component* lc)
{
	Vector2D reflection = reflect(newPos, lc->a, lc->b);
	return reflection;
}

Vector2D ColliderFunctions::PositionToReturnToAfterCollision(Vector2D from, Vector2D to, Vector2D pos, double percentage, const Vector2D* intersectionPoint)
{
	//Doesn't work for rotated rectangles
	Vector2D point = percentage > 0.5f ? to : from;

	Vector2D neededMovement = *intersectionPoint - point;
	return pos + neededMovement;
}