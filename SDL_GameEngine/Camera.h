#pragma once
#include "Vector2D.h"
#include "SDL.h"

namespace Engine
{
	struct Camera_Component
	{
		int w, h;
		double dampingX;
		double dampingY;
		Vector2D offset;
		double* size;

		Vector2D lastPos;
		Vector2D pos;
		SDL_Rect resultRect;
	};

	class Camera_System
	{
	public:
		void Init(class Registry* reg);
		void Update(class Registry* reg, double* deltaTime);
	};
}