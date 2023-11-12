#include "SoftbodyJoint.h"
#include "Registry.h"
#include "Entity.h"
#include "Game.h"

namespace Engine
{
	void SoftbodyJoint_System::Init(Registry* reg, const Game* game)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->softbodyJoints.count(e))
			{
				for (int x = 0; x < reg->softbodyJoints[e].values.size(); x++)
				{
					SoftbodyJoint_Component* c = &reg->softbodyJoints[e];
					if (reg->softbodyJoints[e].values[x].otherBodyAnchorPoints.size() == 0)
					{
						SoftbodyJoint_Component* c = &reg->softbodyJoints[e];
						for (int i = 0; i < c->values[x].otherBody->massPointsN; i++)
						{
							c->values[x].otherBodyAnchorPoints.push_back(&c->values[x].otherBody->massPoints[i]);
						}
					}
					if (reg->softbodyJoints[e].values[x].thisBodyAnchorPoints.size() == 0 && reg->softbodies.count(e))
					{
						SoftbodyJoint_Component* c = &reg->softbodyJoints[e];
						for (int i = 0; i < reg->softbodies[e].massPointsN; i++)
						{
							c->values[x].thisBodyAnchorPoints.push_back(&reg->softbodies[e].massPoints[i]);
						}
					}

					{
						Vector2D moveAverage = Vector2D(0, 0), staticAverage = Vector2D(0, 0);
						for (int i = 0; i < c->values[x].otherBodyAnchorPoints.size(); i++)
						{
							moveAverage += c->values[x].otherBodyAnchorPoints[i]->position;
						}
						moveAverage /= c->values[x].otherBodyAnchorPoints.size();

						for (int i = 0; i < c->values[x].thisBodyAnchorPoints.size(); i++)
						{
							staticAverage += c->values[x].thisBodyAnchorPoints[i]->position;
						}
						staticAverage /= c->values[x].thisBodyAnchorPoints.size();
						staticAverage += c->values[x].offsetFromStaticBody;

						Vector2D dif = staticAverage - moveAverage;

						for (int i = 0; i < c->values[x].otherBody->massPointsN; i++)
						{
							c->values[x].otherBody->massPoints[i].position += dif;
						}
					}

				}
			}
		}
	}

	void SoftbodyJoint_System::Update(Registry* reg, double* deltaTime)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->softbodyJoints.count(e))
			{
				SoftbodyJoint_Component* c = &reg->softbodyJoints[e];
				for (int x = 0; x < reg->softbodyJoints[e].values.size(); x++)
				{
					if (c->values[x].tp == StaticJoint)
					{
						Vector2D moveAverage = Vector2D(0, 0), staticAverage = Vector2D(0, 0);
						for (int i = 0; i < c->values[x].otherBodyAnchorPoints.size(); i++)
						{
							moveAverage += c->values[x].otherBodyAnchorPoints[i]->position;
						}
						moveAverage /= c->values[x].otherBodyAnchorPoints.size();

						for (int i = 0; i < c->values[x].thisBodyAnchorPoints.size(); i++)
						{
							staticAverage += c->values[x].thisBodyAnchorPoints[i]->position;
						}
						staticAverage /= c->values[x].thisBodyAnchorPoints.size();

						Vector2D dif = (staticAverage + c->values[x].offsetFromStaticBody) - moveAverage;

						for (int i = 0; i < c->values[x].otherBody->massPointsN; i++)
						{
							std::cout << staticAverage.x << " " << staticAverage.y << "\n";
							std::cout << moveAverage.x << " " << moveAverage.y << "\n";
							std::cout << dif.x << " " << dif.y << "\n";
							c->values[x].otherBody->massPoints[i].position += dif;
						}
					}
					else if (c->values[x].tp == SpringJoint)
					{
						Vector2D AAverage = Vector2D(0, 0), BAverage = Vector2D(0, 0);
						for (int i = 0; i < c->values[x].otherBodyAnchorPoints.size(); i++)
						{
							AAverage += c->values[x].otherBodyAnchorPoints[i]->position;
						}
						AAverage /= c->values[x].otherBodyAnchorPoints.size();

						for (int i = 0; i < c->values[x].thisBodyAnchorPoints.size(); i++)
						{
							BAverage += c->values[x].thisBodyAnchorPoints[i]->position;
						}
						BAverage /= c->values[x].thisBodyAnchorPoints.size();
						BAverage += c->values[x].offsetFromStaticBody;

						Vector2D averageVelocityA = Vector2D(0, 0);
						for (int i = 0; i < c->values[x].otherBody->massPointsN; i++)
						{
							averageVelocityA += c->values[x].otherBody->massPoints[i].velocity;
						}
						averageVelocityA /= c->values[x].otherBody->massPointsN;

						Vector2D averageVelocityB = Vector2D(0, 0);
						for (int i = 0; i < reg->softbodies[e].massPointsN; i++)
						{
							averageVelocityB += reg->softbodies[e].massPoints[i].velocity;
						}
						averageVelocityB /= reg->softbodies[e].massPointsN;

						Vector2D forceA, forceB;
						CalculateSpringForce(AAverage, BAverage, 3000, 0, 10, averageVelocityA, averageVelocityB, &forceA, &forceB, deltaTime);
						for (int i = 0; i < c->values[x].otherBody->massPointsN; i++)
						{
							c->values[x].otherBody->massPoints[i].force += forceA;
						}
						for (int i = 0; i < reg->softbodies[e].massPointsN; i++)
						{
							reg->softbodies[e].massPoints[i].force += forceB;
						}
						for (int i = 0; i < c->values[x].otherBodyAnchorPoints.size(); i++)
						{
							c->values[x].otherBodyAnchorPoints[i]->force += forceA;
						}

						for (int i = 0; i < c->values[x].thisBodyAnchorPoints.size(); i++)
						{
							c->values[x].thisBodyAnchorPoints[i]->force += forceB;
						}

					}
				}
			}
		}
	}
	void SoftbodyJoint_System::CalculateSpringForce(Vector2D APosition, Vector2D BPosition, float stiffness, float restLength, float dampingFactor, Vector2D velocityA, Vector2D velocityB, Vector2D* forceA, Vector2D* forceB, double* deltaTime)
	{
		double length = (BPosition - APosition).length();
		double f = stiffness * (length - restLength);

		Vector2D normalizedDir = (BPosition - APosition) / (length == 0 ? 1 : length);
		Vector2D velDif = velocityB - velocityA;

		double dot = Vector2D::DotProduct(normalizedDir, velDif);
		double damp = dot * dampingFactor;

		double fullForce = f + damp;

		*forceA = normalizedDir * fullForce;
		*forceB = normalizedDir * fullForce * -1;
	}
}
