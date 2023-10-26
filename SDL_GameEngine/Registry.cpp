#include "Registry.h"
#include "Game.h"

Registry* Registry::instance = NULL;

Registry* Registry::Instance()
{
	if (instance == nullptr)
	{
		instance = new Registry();
	}
	return instance;
}

void Registry::Init(SDL_Renderer* renderer)
{
	fluidSimulationSystem.Init(this);
	rigidbodySystem.Init(this);
	shapeSystem.Init(this);
	colliderSystem.Init(this);
	sliderBoxSystem.Init(this, renderer);
	buttonSystem.Init(this, renderer);
	textSystem.Init(this, renderer);
	textBoxSystem.Init(this, renderer);
	softbodySystem.Init(this);
	tiledSpriteSystem.Init(this, renderer);
}

void Registry::Update(double* deltaTime, Game* game)
{
	if (!game->stopSimulation)
	{
		rigidbodySystem.Update(this, deltaTime);
		fluidSimulationSystem.Update(deltaTime);
	}
	softbodySystem.Update(this, deltaTime, game);
	textBoxSystem.Update(this, deltaTime, game);
	sliderBoxSystem.Update(this, deltaTime, game);
	buttonSystem.Update(this, deltaTime, game);
}

void Registry::Draw(SDL_Renderer* renderer)
{
	tiledSpriteSystem.Draw(this, renderer);
	fluidSimulationSystem.Draw(renderer);
	textSystem.Draw(this, renderer);
	textBoxSystem.Draw(this, renderer);
#ifdef _DEBUG
	rigidbodySystem.Draw(this, renderer);
	colliderSystem.Draw(this, renderer);
#endif // _DEBUG
	softbodySystem.Draw(this, renderer);
	shapeSystem.Draw(this, renderer);
}