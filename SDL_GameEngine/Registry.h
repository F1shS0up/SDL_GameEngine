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
#include "Softbody.h"
#include "Button.h"
#include "TiledSprite.h"
#include <array>
#include "SoftbodyRotatorMovement.h"
#include "SoftbodyJoint.h"
#include "Camera.h"
#include "RandomSoftbodyGenerator.h"
#include "Sprite.h"
#include "SpriteSheet.h"
#include "SpriteSheetAnimator.h"
#include "Tilemap.h"
#include "RigidCharacterMovement.h"

namespace Engine
{
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
		std::unordered_map<Entity, Softbody_Component> softbodies;
		std::unordered_map<Entity, Button_Component> buttons;
		std::unordered_map<Entity, TiledSprite_Component> tiledSprites;
		std::unordered_map<Entity, SoftbodyRotatingMovement_Component> softbodyRotatingMovements;
		std::unordered_map<Entity, SoftbodyJoint_Component> softbodyJoints;
		std::unordered_map<Entity, Camera_Component> cameras;
		std::unordered_map<Entity, RandomSoftbodyGenerator_Component> randomSoftbodyGenerators;
		std::unordered_map<Entity, Sprite_Component> sprites;
		std::unordered_map<Entity, SpriteSheet_Component> spriteSheets;
		std::unordered_map<Entity, SpriteSheetAnimator_Component> spriteSheetAnimators;
		std::unordered_map<Entity, Tilemap_Component> tilemaps;
		std::unordered_map<Entity, RigidCharacterMovement_Component> rigidCharacterMovements;

#pragma endregion

#pragma region systems

		Rigidbody_System rigidbodySystem;
		Shape_System shapeSystem;
		Collider_System colliderSystem;
		FluidSimulation_System fluidSimulationSystem;
		Text_System textSystem;
		TextBox_System textBoxSystem;
		SliderBox_System sliderBoxSystem;
		Softbody_System softbodySystem;
		Button_System buttonSystem;
		TiledSprite_System tiledSpriteSystem;
		SoftbodyRotatingMovement_System softbodyRotatingMovementSystem;
		SoftbodyJoint_System softbodyJointSystem;
		Camera_System camSystem;
		RandomSoftbodyGenerator_System randomSoftbodyGeneratorSystem;
		Sprite_System spriteSystem;
		SpriteSheet_System spriteSheetSystem;
		SpriteSheetAnimator_System spriteSheetAnimatorSystem;
		Tilemap_System tilemapSystem;
		RigidCharacterMovement_System rigidCharacterMovementSystem;

#pragma endregion

		void Init(SDL_Renderer* renderer, const class Game* game);
		void StartUpdate(double* deltaTime, class Game* game);
		void Update(double* deltaTime, class Game* game);
		void Draw(SDL_Renderer* renderer, const SDL_Rect* cameraRect, const class Game* game);

	private:

		static Registry* instance;
	};
}
