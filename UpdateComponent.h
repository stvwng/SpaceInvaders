#pragma once
#include "Component.h"

class UpdateComponent : public Component
{
    private:
        bool m_Enabled = false;

    public:
        virtual void update(float dt) = 0;

        // implement virtual functions from Component interface
        ComponentType getType() const override
        {
            return ComponentType::Update;
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

        void start(GameObjectSharer*, GameObject*) override {}
};