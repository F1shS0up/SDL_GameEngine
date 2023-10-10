#pragma once
#include <iostream>

struct EntityManager
{
	static EntityManager* Instance();
	// an alias to make it more expressive
	using Entity = std::size_t;
	// we need the maximum count, this isn't the best solution, but it does the job for now
	const Entity max_entity = 5000;

	Entity num_entities = 0;

	// and here we create some unique ID's
	std::size_t CreateEntity();
	void DestroyEntity(std::size_t entity);

private:
	static EntityManager* instance;
};