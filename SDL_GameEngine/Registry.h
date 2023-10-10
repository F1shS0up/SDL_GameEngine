#pragma once
#include <unordered_map>
#include "Rigidbody.h"
#include "Shape.h"
#include "Transform.h"
#include "Collider.h"
#include <array>

using Entity = std::size_t;


struct Registry
{
public:
    static Registry* Instance();

#pragma region components

    std::unordered_map<Entity, Rigidbody_Component> rigidbodies;
    std::unordered_map<Entity, CircleFilled_Component> circlesFilled;
    std::unordered_map<Entity, Transform_Component> transforms;
    std::unordered_map<Entity, CircleCollider_Component> circleCollider;
    std::unordered_map<Entity, LineCollider_Component> lineCollider;

#pragma endregion

#pragma region systems

    Rigidbody_System rigidbodySystem;
    Shape_System shapeSystem;
    Collider_System colliderSystem;

#pragma endregion

    void Init();
    void Update(double* deltaTime);
    void Draw(SDL_Renderer* renderer);

private:

    static Registry* instance;
};

