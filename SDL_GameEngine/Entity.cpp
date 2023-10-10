#include "Entity.h"
#include "Registry.h"

EntityManager* EntityManager::instance = NULL;

EntityManager* EntityManager::Instance()
{
    if (instance == nullptr)
    {
        instance = new EntityManager();
    }
    return instance;
}

std::size_t EntityManager::CreateEntity()
{
    ++num_entities;
    return num_entities;
}

void EntityManager::DestroyEntity(std::size_t entity)
{
    --num_entities;
    Registry::Instance()->rigidbodies.erase(entity);
}


