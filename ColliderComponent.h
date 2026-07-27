#pragma once
#include "Component.h"
#include <iostream>

class ColliderComponent : public Component
{
    private:
        string m_Type = "collider";
        bool m_Enabled = false;

    public:
        // From Component Interface
        // Override virtual functions

        string getType() override
        {
            return m_Type;
        }

        void disableComponent() override
        {
            m_Enabled = false;
        }

        void enableComponent() override
        {
            m_Enabled = true;
        }

        bool enabled() override
        {
            return m_Enabled;
        }

        void start(GameObjectSharer*, GameObject*) override
        {
            // code to be added later
        }
};