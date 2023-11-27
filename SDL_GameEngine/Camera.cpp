#include "Camera.h"
#include "Registry.h"
#include "Entity.h"
namespace Engine
{
	void Camera_System::Init(Registry* reg)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->cameras.count(e) && reg->transforms.count(e))
			{
				Camera_Component* c = &reg->cameras[e];

				c->pos.x = reg->transforms[e].position.x - c->w / 2;
				c->pos.y = reg->transforms[e].position.y - c->h / 2;
				c->lastPos = c->pos;

				c->resultRect = SDL_Rect{ (int)c->pos.x, (int)c->pos.y, c->w, c->h };
			}
		}
	}

	void Camera_System::Update(Registry* reg, double* deltaTime)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->cameras.count(e) && reg->transforms.count(e))
			{
				Camera_Component* c = &reg->cameras[e];
				c->lastPos = c->pos;

				Vector2D newCameraPos = c->lastPos;

				newCameraPos.x = reg->transforms[e].position.x - c->w / 2;
				newCameraPos.y = reg->transforms[e].position.y - c->h / 2;
				c->pos.x = c->lastPos.x + (newCameraPos.x - c->lastPos.x) / c->dampingX;
				c->pos.y = c->lastPos.y + (newCameraPos.y - c->lastPos.y) / c->dampingY;

				c->resultRect = SDL_Rect{ (int)c->pos.x, (int)c->pos.y, c->w, c->h };
			}
		}
	}
}