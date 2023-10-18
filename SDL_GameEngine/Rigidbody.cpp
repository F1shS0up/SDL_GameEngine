#include "Rigidbody.h"
#include "Entity.h"
#include "Registry.h"
#include <cmath>
#include <numeric>

#define ComputeVelocityForCircles(v1, v2, x1, x2, m1, m2, out)\
 *out = v1 - Vector2D((2 * m2 / (m1 + m2)) * Vector2D::DotProduct(v1 - v2, x1 - x2) / ((x1 - x2).length() * (x1 - x2).length())* (x1 - x2).x, (2 * m2 / (m1 + m2)) * Vector2D::DotProduct(v1 - v2, x1 - x2) / ((x1 - x2).length() * (x1 - x2).length())* (x1 - x2).y)\

void Rigidbody_Component::ApplyForce(Vector2D force)
{
	if (!isStatic)
	{
		//Use GetMass() and GetMoment() here in case we ever need them to do something more than just return their values
		//Calculate the object's acceleration
		Vector2D acceleration = force / mass;
		//Add acceleration to object's velocity
		velocity += acceleration;
	}
}

#pragma region System

void Rigidbody_System::Init(Registry* reg)
{
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->rigidbodies.count(e))
		{
			Rigidbody_Component* c = &reg->rigidbodies[e];

			c->inverseMass = 1 / c->mass;
			if (reg->transforms.count(e))
			{
				Transform_Component* cT = &reg->transforms[e];
				c->position = &cT->position;
			}
		}
	}
}
#ifdef _DEBUG
void Rigidbody_System::Draw(Registry* reg, SDL_Renderer* renderer)
{
}
#endif

Vector2D Rigidbody_System::ReflectionResponse(Vector2D* normal, Vector2D* velocity)
{
	// u = v * n * n * n
	float dot = Vector2D::DotProduct(*velocity, *normal);

	Vector2D u = Vector2D(dot * normal->x, dot * normal->y);
	Vector2D w = *velocity - u;
	// w = v - u
	// new v = w - u

	// (v - v * n * n * n) - v * n * n * n

	Vector2D result = w - u;

	return result;
}

void Rigidbody_System::ResolveCollision(Rigidbody_Component* A, Rigidbody_Component* B, Vector2D normal)
{
	//Calculate relative velocity between two objects -- improve by determining total velocity (linear + rotational velocity)
	Vector2D relativeVelocity = B->velocity - A->velocity;

	Vector2D force;

	if (A->isStatic && !B->isStatic)
	{
		float j = Vector2D::DotProduct((relativeVelocity) * -(1 + A->elasticity), normal) / (1 / B->mass);
		force = normal * j;
		B->ApplyForce(force);
	}
	else if (B->isStatic && !A->isStatic)
	{
		float j = Vector2D::DotProduct((relativeVelocity) * -(1 + A->elasticity), normal) / ((1 / A->mass));
		force = normal * j;
		A->ApplyForce(force * -1);
	}
	else
	{
		float j = Vector2D::DotProduct((relativeVelocity) * -(1 + A->elasticity), normal) / ((1 / A->mass) + (1 / B->mass));
		force = normal * j;
		A->ApplyForce(force * -1);
		B->ApplyForce(force);
	}
}

void Rigidbody_System::PositionalCorrection(Rigidbody_Component* A, Rigidbody_Component* B, float penetration, Vector2D collisionNormal)
{
	*A->position += collisionNormal * std::abs(penetration);
}
void Rigidbody_System::Update(Registry* reg, double* deltaTime)
{
int x = 0;
	for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
	{
		if (reg->rigidbodies.count(e))
		{
			Rigidbody_Component* c = &reg->rigidbodies[e];
			c->force = Vector2D(0, 0);
			c->force.y = *c->gravity;

			//Rigidbody properties to make it look better
			float m1 = c->mass;
			c->velocity = c->velocity + c->force * *deltaTime;

			Vector2D v1 = c->velocity;
			Vector2D x1 = *c->position;
			Vector2D upcomingX1 = x1 + v1 * *deltaTime;

			//Update the basic velocity, acceleration is usually gravity

			if (reg->circleColliders.count(e))
			{
				CircleCollider_Component* cc = &reg->circleColliders[e];

				for (int e2 = e + 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->rigidbodies.count(e2))
					{
						Rigidbody_Component* c2 = &reg->rigidbodies[e2];

						//Basic rigidbody properties again for pretty look
						Vector2D v2 = c2->velocity;
						Vector2D x2 = *c2->position;
						Vector2D upcomingX2 = x2 + v2 * *deltaTime;
						float m2 = c2->mass;

						if (!(c2->ignoreLayers & c->layer) && !(c->ignoreLayers & c2->layer))
						{
							if (reg->circleColliders.count(e2))
							{
								CircleCollider_Component* cc2 = &reg->circleColliders[e2];

								float penetration = 0;

								if (ColliderFunctions::CircleWithCircleIntersection(upcomingX1, cc2->radius, upcomingX2, cc->radius, &penetration))
								{
									ResolveCollision(c, c2, (upcomingX2 - upcomingX1).normalize());
								}
							}
						}
					}
				}
				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->lineColliders.count(e2))
					{
						LineCollider_Component* cl = &reg->lineColliders[e2];

						Vector2D pointOfIntersection;
						bool intersects = ColliderFunctions::CircleWithLineIntersection(cl, *cc->position, cc->radius, &pointOfIntersection);
						if (intersects)
						{
							//Collided with line

							//Get the normal which is closer to the middle
							Vector2D normal = ColliderFunctions::ReflectionNormal(cl, *cc->position);
							//Calculate the response
							c->velocity = ReflectionResponse(&normal, &v1) * c->elasticity;
							//Make sure that the position is awway from line so that the collision doesn't happen twice
							*c->position = ColliderFunctions::PositionToReturnToAfterCollision(&normal, cc, pointOfIntersection);
						}
					}
					if (reg->rigidbodies.count(e2))
					{
						Rigidbody_Component* c2 = &reg->rigidbodies[e2];

						Vector2D v2 = c2->velocity;
						Vector2D x2 = *c2->position;
						Vector2D upcomingX2 = x2 + v2 * *deltaTime;
						float m2 = c2->mass;

						if (!(c2->ignoreLayers & c->layer) && !(c->ignoreLayers & c2->layer))
						{
							if (reg->AABBColliders.count(e2))
							{
								AABBCollider_Component* ac = &reg->AABBColliders[e2];

								if (ColliderFunctions::CircleWithRectangleIntersection(upcomingX1, cc->radius, upcomingX2, ac->width, ac->height))
								{
									ResolveCollision(c, c2, (upcomingX2 + Vector2D(ac->width / 2, ac->height / 2) - upcomingX1).normalize());
									std::cout << (upcomingX2 - upcomingX1).normalize().x << " " << (upcomingX2 - upcomingX1).normalize().y << std::endl;
								}
							}
						}
					}
				}
			}
			if (reg->AABBColliders.count(e))
			{
				AABBCollider_Component* ac = &reg->AABBColliders[e];

				for (int e2 = 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->lineColliders.count(e2))
					{
						LineCollider_Component* cl = &reg->lineColliders[e2];

						Vector2D intersectionPoint, fromIntersectedLine, toIntersectedLine;
						float outA;
						if (ColliderFunctions::RectangleWithLineIntersection(ac->width, ac->height, x1, cl->a, cl->b, &intersectionPoint, &fromIntersectedLine, &toIntersectedLine, &outA))
						{
							Vector2D normal = ColliderFunctions::ReflectionNormal(cl, x1);
							c->velocity = ReflectionResponse(&normal, &v1) * c->elasticity;

							//Set the position away from the line so that it wont collide again which could result in a rectangle being stuck in line
							*c->position = ColliderFunctions::PositionToReturnToAfterCollision(fromIntersectedLine, toIntersectedLine, x1, outA, &intersectionPoint);
						}
					}
				}
				for (int e2 = e + 1; e2 <= EntityManager::Instance()->num_entities; e2++)
				{
					if (reg->rigidbodies.count(e2))
					{
						Rigidbody_Component* c2 = &reg->rigidbodies[e2];

						//Basic rigidbody properties again for pretty look
						Vector2D v2 = c2->velocity;
						Vector2D x2 = *c2->position;
						Vector2D upcomingX2 = x2 + v2 * *deltaTime;
						float m2 = c2->mass;
						if (!(c2->ignoreLayers & c->layer) && !(c->ignoreLayers & c2->layer))
						{
							if (reg->AABBColliders.count(e2))
							{
								AABBCollider_Component* ac2 = &reg->AABBColliders[e2];
								Vector2D normal;
								float penetration;
								if (ColliderFunctions::RectangleWithRectangleIntersection(ac->width, ac->height, upcomingX1, ac2->width, ac2->height, upcomingX2, &normal, &penetration))
								{
									ResolveCollision(c, c2, normal);

								}
							}
						}
					}
				}
			}

			*c->position = *c->position + c->velocity * *deltaTime;
		}
	}
}

#pragma endregion