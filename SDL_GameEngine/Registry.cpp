#include "Registry.h"
#include "Game.h"
namespace Engine
{
	Registry* Registry::instance = NULL;

	Registry* Registry::Instance()
	{
		if (instance == nullptr)
		{
			instance = new Registry();
		}
		return instance;
	}

	void Registry::Init(SDL_Renderer* renderer, const Game* game)
	{
		tilemapSystem.Init(this, renderer);
		randomSoftbodyGeneratorSystem.Init(this, game);
		fluidSimulationSystem.Init(this);
		rigidbodySystem.Init(this);
		shapeSystem.Init(this);
		colliderSystem.Init(this);
		sliderBoxSystem.Init(this, renderer);
		buttonSystem.Init(this, renderer);
		textSystem.Init(this, renderer);
		textBoxSystem.Init(this, renderer);
		softbodySystem.Init(this, game);
		tiledSpriteSystem.Init(this, renderer);
		softbodyRotatingMovementSystem.Init(this);
		softbodyJointSystem.Init(this, game);
		camSystem.Init(this);
		spriteSystem.Init(this, renderer);
		spriteSheetSystem.Init(this, renderer);
		rigidCharacterMovementSystem.Init(this);
	}

	void Registry::StartUpdate(double* deltaTime, Game* game)
	{
		softbodySystem.StartUpdate(this);
	}

	void Registry::Update(double* deltaTime, Game* game)
	{
		rigidCharacterMovementSystem.Update(this, deltaTime);
		rigidbodySystem.Update(this, deltaTime);
		fluidSimulationSystem.Update(deltaTime);
		softbodyRotatingMovementSystem.Update(this, deltaTime);
		softbodyJointSystem.Update(this, deltaTime);
		softbodySystem.Update(this, deltaTime, game);
		textBoxSystem.Update(this, deltaTime, game);
		sliderBoxSystem.Update(this, deltaTime, game);
		buttonSystem.Update(this, deltaTime, game);
		camSystem.Update(this, deltaTime);
		spriteSheetAnimatorSystem.Update(this, deltaTime);
	}

	void Registry::Draw(SDL_Renderer* renderer, const SDL_Rect* cameraRect, const Game* game)
	{
		tilemapSystem.Draw(this, renderer);
		tiledSpriteSystem.Draw(this, renderer, cameraRect);
		fluidSimulationSystem.Draw(renderer, cameraRect);
		textSystem.Draw(this, renderer, cameraRect);
		textBoxSystem.Draw(this, renderer, cameraRect);

		rigidbodySystem.Draw(this, renderer, cameraRect);

		colliderSystem.Draw(this, renderer, cameraRect);
		softbodySystem.Draw(this, renderer, cameraRect, game);
		shapeSystem.Draw(this, renderer, cameraRect);
		spriteSystem.Draw(this, renderer);
		spriteSheetSystem.Draw(this, renderer);
	}
}