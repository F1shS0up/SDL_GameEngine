#include "Registry.h"

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
	textSystem.Init(this, renderer);
}

void Registry::Update(double* deltaTime)
{
	rigidbodySystem.Update(this, deltaTime);
	fluidSimulationSystem.Update(deltaTime);
	
}

void Registry::Draw(SDL_Renderer* renderer)
{
	shapeSystem.Draw(this, renderer);
	fluidSimulationSystem.Draw(renderer);
	textSystem.Draw(this, renderer);
#ifdef _DEBUG
	rigidbodySystem.Draw(this, renderer);
	colliderSystem.Draw(this, renderer);
#endif // _DEBUG
}