#include "Entity.h"
#include "Registry.h"
namespace Engine
{
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
		Registry::Instance()->filledCircles.erase(entity);
		Registry::Instance()->filledRectangles.erase(entity);
		Registry::Instance()->transforms.erase(entity);
		Registry::Instance()->circleColliders.erase(entity);
		Registry::Instance()->AABBColliders.erase(entity);
		Registry::Instance()->lineColliders.erase(entity);
		Registry::Instance()->fluids.erase(entity);
		Registry::Instance()->texts.erase(entity);
		Registry::Instance()->textBoxes.erase(entity);
		Registry::Instance()->sliderBoxes.erase(entity);
		Registry::Instance()->softbodies.erase(entity);
		Registry::Instance()->buttons.erase(entity);
		Registry::Instance()->tiledSprites.erase(entity);
	}
}