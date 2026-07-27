#pragma once
#include "UpdateComponent.h"
#include "TransformComponent.h"
#include "GameObjectSharer.h"
#include "RectColliderComponent.h"
#include "GameObject.h"
#include "SoundPlayer.h"

class BulletSpawner;

class InvaderUpdateComponent : public UpdateComponent
{
    private:
        string m_SpecificType = "invader";

        shared_ptr<TransformComponent> m_TC;
        shared_ptr<RectColliderComponent> m_RCC;
        shared_ptr<TransformComponent> m_PlayerTC;
        shared_ptr<RectColliderComponent> m_PlayerRCC;

        BulletSpawner* m_BulletSpawner = nullptr;
        SoundPlayer* m_SoundPlayer = nullptr;

        // Shots can never come closer together than this, however high the
        // wave number climbs.
        static constexpr float MIN_TIME_BETWEEN_SHOTS = 0.25f;

        float m_Speed = 10.0f;
        bool m_MovingRight = true;
        float m_TimeSinceLastShot = 0.f;
        float m_TimeBetweenShots = 5.0f;
        float m_AccuracyModifier = 0.f;
        float m_SpeedModifier = 0.05f;

    public:
        void dropDownAndReverse();
        bool isMovingRight();
        void initializeBulletSpawner(BulletSpawner* bulletSpawner, int indexInLevel, SoundPlayer& soundPlayer);

        // From Component interface
        string getSpecificType() override
        {
            return m_SpecificType;
        }

        void start(GameObjectSharer* gos, GameObject* self) override
        {
            // Where is the Player?
            m_PlayerTC = static_pointer_cast<TransformComponent>(
                gos->findFirstObjectWithTag("Player").getComponentByTypeAndSpecificType("transform", "transform")
            );

            m_PlayerRCC = static_pointer_cast<RectColliderComponent>(
                gos->findFirstObjectWithTag("Player").getComponentByTypeAndSpecificType("collider", "rect")
            );

            // Where is this specific Invader?
            m_TC = static_pointer_cast<TransformComponent>(
                self->getComponentByTypeAndSpecificType("transform", "transform")
            );

            m_RCC = static_pointer_cast<RectColliderComponent>(
                self->getComponentByTypeAndSpecificType("collider", "rect")
            );

        }

        // From UpdateComponent
        void update(float dt) override;
};
