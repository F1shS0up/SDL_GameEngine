#include "Collider.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include <numeric>
#include <complex>
void Collider_System::Init(Registry* reg)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->circleCollider.count(e))
		{
			CircleCollider_Component* c = &reg->circleCollider[e];

			if (reg->transforms.count(e))
			{
				Transform_Component* cT = &reg->transforms[e];
				c->position = &cT->position;
			}
		}
		if (reg->lineCollider.count(e))
		{
			LineCollider_Component* c = &reg->lineCollider[e];
			float da = c->b.x - c->a.x;
			float db = c->b.y - c->a.y;

			c->normal = Vector2D(-db, da).normalize();

			c->mid = Vector2D(c->a.x + (c->b.x - c->a.x) / 2, c->a.y + (c->b.y - c->a.y) / 2);
		}
	}
}

#ifdef _DEBUG

void Collider_System::Draw(Registry* reg, class SDL_Renderer* renderer)
{

	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->lineCollider.count(e))
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
			SDL_RenderDrawLine(renderer, reg->lineCollider[e].a.x, reg->lineCollider[e].a.y, reg->lineCollider[e].b.x, reg->lineCollider[e].b.y);

			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 1);
			SDL_RenderDrawLine(renderer, reg->lineCollider[e].mid.x, reg->lineCollider[e].mid.y, reg->lineCollider[e].mid.x + reg->lineCollider[e].normal.x * 200, reg->lineCollider[e].mid.y + reg->lineCollider[e].normal.y * 200);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
			SDL_RenderDrawLine(renderer, reg->lineCollider[e].mid.x, reg->lineCollider[e].mid.y, reg->lineCollider[e].mid.x - reg->lineCollider[e].normal.x * 200, reg->lineCollider[e].mid.y - reg->lineCollider[e].normal.y * 200);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);

			for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
			{
				if (reg->circleCollider.count(e2))
				{
					LineCollider_Component* cl = &reg->lineCollider[e];
					CircleCollider_Component* cc = &reg->circleCollider[e2];

					Vector2D intersection;
					bool inter = ColliderFunctions::LineCollision(*cc->position + cl->normal * cc->radius, *cc->position - cl->normal * cc->radius, cl->a, cl->b, &intersection);
					if (inter)
					{
						ShapesRendering::DrawFilledCircle(renderer, intersection.x, intersection.y, 20, SDL_Color{ 255, 255, 0, 1 });
					}

				}
			}
		}
	}
}

#endif // _DEBUG
inline double Dot(const Vector2D& a, const Vector2D& b) { return (a.x * b.x) + (a.y * b.y); }
inline double PerpDot(const Vector2D& a, const Vector2D& b) { return (a.y * b.x) - (a.x * b.y); }
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


bool ColliderFunctions::LineCollision(const Vector2D& A1, const Vector2D& A2, const Vector2D& B1, const Vector2D& B2, Vector2D* intersection)
{
	Vector2D a(A2 - A1);
	Vector2D b(B2 - B1);

	double f = PerpDot(a, b);
	if (!f)      // lines are parallel
		return false;

	Vector2D c(B2 - A2);
	double aa = PerpDot(a, c);
	double bb = PerpDot(b, c);

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
	return true;
}

bool ColliderFunctions::CircleWithLineIntersection(LineCollider_Component* lineCollider, Vector2D position, float radius, Vector2D* intersectionPoint)
{
	Vector2D a1, a2, b1, b2;
	a1 = lineCollider->a;
	b1 = lineCollider->b;

	a2 = position + lineCollider->normal * radius;
	// b2 is equal to the center of the circle and normal of line times radius
	b2 = position - lineCollider->normal * radius;

	Vector2D intersection;
	bool intersects = LineCollision(a1, b1, a2, b2, &intersection);

	*intersectionPoint = intersection;

	return intersects;
}

bool ColliderFunctions::FrameIndependentCircleWithLineIntersection(LineCollider_Component* lineCollider, CircleCollider_Component* circleCollider, Vector2D nextFramePosToAdd, Vector2D* intersectionPoint)
{
	bool intersect = CircleWithLineIntersection(lineCollider, *circleCollider->position + nextFramePosToAdd, circleCollider->radius, intersectionPoint);
	if (intersect)
	{
		return true;
	}

	Vector2D a1, a2, b1, b2;
	a1 = lineCollider->a;
	b1 = lineCollider->b;

	a2 = *circleCollider->position + nextFramePosToAdd;
	// b2 is equal to the center of the circle and normal of line times radius
	b2 = *circleCollider->position;
	intersect = LineCollision(a1, b1, a2, b2, intersectionPoint);
	if (intersect)
	{
		return true;
	}

	
	return false;
}

bool ColliderFunctions::CircleWithCircleIntersection(Vector2D posA, float radiusA, Vector2D posB, float radiusB)
{
	Vector2D difference = Vector2D(std::abs(posB.x - posA.x), std::abs(posB.y - posA.y));
	float distance = difference.length();
	
	return distance < radiusA + radiusB;
}

Vector2D ColliderFunctions::ReflectionNormal(LineCollider_Component* lineCollider, Vector2D point)
{
	Vector2D normalA, normalB;

	normalA = lineCollider->mid + lineCollider->normal;
	normalB = lineCollider->mid - lineCollider->normal;

	float lengthA = (normalA - point).length();
	float lengthB = (normalB - point).length();



	return (lengthA < lengthB) ? lineCollider->normal : Vector2D(-lineCollider->normal.x, -lineCollider->normal.y);
}

Vector2D ColliderFunctions::PositionToReturnToAfterCollision(Vector2D* normal, CircleCollider_Component* circleCollider, Vector2D intersection)
{
	Vector2D pos = *circleCollider->position;
	Vector2D biggestPos = pos - Vector2D(circleCollider->radius * normal->x, circleCollider->radius * normal->y);
	Vector2D result = (biggestPos - intersection) * -1;
	return result + pos;
}
Vector2D ColliderFunctions::PositionToReturnToAfterCollision(Vector2D* normal, Vector2D pos, Vector2D newPos, float radius, Vector2D intersection, LineCollider_Component lc)
{
	Vector2D reflection = reflect(newPos, lc.a, lc.b);
	return reflection;

}