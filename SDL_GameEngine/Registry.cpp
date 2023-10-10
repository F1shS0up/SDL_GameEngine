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

void Registry::Init()
{
    rigidbodySystem.Init(this);
    shapeSystem.Init(this);
    colliderSystem.Init(this);
}

void Registry::Update(double* deltaTime)
{
    rigidbodySystem.Update(this, deltaTime);
}

void Registry::Draw(SDL_Renderer* renderer)
{
    shapeSystem.Draw(this, renderer);
#ifdef _DEBUG
    rigidbodySystem.Draw(this, renderer);
    colliderSystem.Draw(this, renderer);
#endif // _DEBUG
}
