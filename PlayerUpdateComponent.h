#pragma once
#include "UpdateComponent.h"
#include "TransformComponent.h"
#include "GameObjectSharer.h"
#include "RectColliderComponent.h"
#include "GameObject.h"

class PlayerUpdateComponent : public UpdateComponent
{
    private:

        std::shared_ptr<TransformComponent> m_TC;
        std::shared_ptr<RectColliderComponent> m_RCC;

        float m_Speed = 50.0f;
        float m_XExtent = 0;
        float m_YExtent = 0;

        bool m_IsHoldingLeft = false;
        bool m_IsHoldingRight = false;
        bool m_IsHoldingUp = false;
        bool m_IsHoldingDown = false;

    public:
        void updateShipTravelWithController(float x, float y);
        void moveLeft();
        void moveRight();
        void moveUp();
        void moveDown();
        void stopLeft();
        void stopRight();
        void stopUp();
        void stopDown();

        // From Component interface
        ComponentSpecificType getSpecificType() const override
        {
            return ComponentSpecificType::Player;
        }

        void start(GameObjectSharer*, GameObject* self) override
        {
            m_TC = std::static_pointer_cast<TransformComponent>(self->getComponentByTypeAndSpecificType(ComponentType::Transform, ComponentSpecificType::Transform));
            m_RCC = std::static_pointer_cast<RectColliderComponent>(self->getComponentByTypeAndSpecificType(ComponentType::Collider, ComponentSpecificType::Rect));
        }

        // From UpdateComponent
        void update(float dt) override;
};