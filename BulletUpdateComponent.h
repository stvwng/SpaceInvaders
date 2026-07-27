#pragma once
#include "UpdateComponent.h"
#include "TransformComponent.h"
#include "GameObjectSharer.h"
#include "RectColliderComponent.h"
#include "GameObject.h"

class BulletUpdateComponent : public UpdateComponent
{
    private:
        shared_ptr<TransformComponent> m_TC;
        shared_ptr<RectColliderComponent> m_RCC;

        float m_Speed = 75.0f;

        // Divides the bullet's speed, so it must never be zero. Set on every
        // invader spawn; initialised here so a stray read cannot divide by an
        // indeterminate value.
        int m_AlienBulletSpeedModifier = 5;
        int m_ModifierRandomComponent = 5;
        int m_MinimumAdditionalModifier = 5;

        bool m_MovingUp = true;

    public:
        bool m_BelongsToPlayer = false;
        bool m_IsSpawned = false;

        void spawnForPlayer(Vector2f spawnPosition);
        void spawnForInvader(Vector2f spawnPosition);
        void deSpawn();
        bool isMovingUp();

        // from Component interface
        ComponentSpecificType getSpecificType() const override
        {
            return ComponentSpecificType::Bullet;
        }

        void start(GameObjectSharer*, GameObject* self) override
        {
            // Where is this specific invader?
            m_TC = static_pointer_cast<TransformComponent>(self->getComponentByTypeAndSpecificType(ComponentType::Transform, ComponentSpecificType::Transform));

            m_RCC = static_pointer_cast<RectColliderComponent>(self->getComponentByTypeAndSpecificType(ComponentType::Collider, ComponentSpecificType::Rect));
        }

        // From UpdateComponent
        void update(float dt) override;
};