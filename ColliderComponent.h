#pragma once
#include "Component.h"
#include <iostream>

class ColliderComponent : public Component
{
    private:
        bool m_Enabled = false;

    public:
        // From Component Interface
        // Override virtual functions

        ComponentType getType() const override
        {
            return ComponentType::Collider;
        }

        void disableComponent() override
        {
            m_Enabled = false;
        }

        void enableComponent() override
        {
            m_Enabled = true;
        }

        bool enabled() const override
        {
            return m_Enabled;
        }

        void start(GameObjectSharer*, GameObject*) override
        {
            // code to be added later
        }
};