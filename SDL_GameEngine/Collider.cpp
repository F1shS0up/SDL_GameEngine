#include "Collider.h"
#include "Registry.h"
#include "Entity.h"
#include "Shapes.h"
#include <numeric>

float VectorDotProduct(Vector2D v1, Vector2D v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
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

Vector2D* ColliderFunctions::CircleWithLineIntersection(LineCollider_Component* lineCollider, CircleCollider_Component* circleCollider, Vector2D nextFramePosToAdd)
{
	return nullptr;
}

Vector2D* ColliderFunctions::CircleWithCircleIntersection(CircleCollider_Component* circleCollider1, CircleCollider_Component* circleCollider2)
{
	return nullptr;
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

Vector2D ColliderFunctions::ReflectionResponse(Vector2D* normal, Vector2D* velocity)
{
	// u = v * n * n * n
	float dot = VectorDotProduct(*velocity, *normal);

	Vector2D u = Vector2D(dot * normal->x, dot * normal->y);
	Vector2D w = *velocity - u;
	// w = v - u
	// new v = w - u

	// (v - v * n * n * n) - v * n * n * n

	Vector2D result = w - u;

	return result;
}

Vector2D ColliderFunctions::PositionToReturnToAfterCollision(Vector2D* normal, CircleCollider_Component* circleCollider, Vector2D intersection)
{
	Vector2D pos = *circleCollider->position;
	Vector2D biggestPos = pos - Vector2D(circleCollider->radius * normal->x, circleCollider->radius * normal->y);
	Vector2D result = (biggestPos - intersection) * -1;
	return result + pos;
}
Vector2D ColliderFunctions::PositionToReturnToAfterCollision(Vector2D* normal, Vector2D pos, float radius, Vector2D intersection)
{
	Vector2D biggestPos = pos - Vector2D(radius * normal->x, radius * normal->y);
	Vector2D result = (biggestPos - intersection) * -1;
	return result + pos;
}


