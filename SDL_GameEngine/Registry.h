#pragma once
#include <unordered_map>
#include "Rigidbody.h"
#include "Shape.h"
#include "Transform.h"
#include "Collider.h"
#include "FluidSimulation.h"
#include "Text.h"
#include "TextBox.h"
#include "SliderBox.h"
#include <array>

using Entity = std::size_t;

struct Registry
{
public:
	static Registry* Instance();

#pragma region components

	std::unordered_map<Entity, Rigidbody_Component> rigidbodies;
	std::unordered_map<Entity, FilledCircle_Component> filledCircles;
	std::unordered_map<Entity, FilledRectangle_Component> filledRectangles;
	std::unordered_map<Entity, Transform_Component> transforms;
	std::unordered_map<Entity, CircleCollider_Component> circleColliders;
	std::unordered_map<Entity, AABBCollider_Component> AABBColliders;
	std::unordered_map<Entity, LineCollider_Component> lineColliders;
	std::unordered_map<Entity, Fluid_Component> fluids;
	std::unordered_map<Entity, Text_Component> texts;
	std::unordered_map<Entity, TextBox_Component> textBoxes;
	std::unordered_map<Entity, SliderBox_Component> sliderBoxes;

#pragma endregion

#pragma region systems

	Rigidbody_System rigidbodySystem;
	Shape_System shapeSystem;
	Collider_System colliderSystem;
	FluidSimulation_System fluidSimulationSystem;
	Text_System textSystem;
	TextBox_System textBoxSystem;
	SliderBox_System sliderBoxSystem;

#pragma endregion

	void Init(SDL_Renderer* renderer);
	void Update(double* deltaTime, class Game* game);
	void Draw(SDL_Renderer* renderer);

private:

	static Registry* instance;
};
