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

        string getType()
        {
            return m_Type;
        }

        void disableComponent()
        {
            m_Enabled = false;
        }

        void enableComponent()
        {
            m_Enabled = true;
        }

        bool enabled()
        {
            return m_Enabled;
        }

        void start(GameObjectSharer* gos, GameObject* self)
        {
            // code to be added later
        }
};