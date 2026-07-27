#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include <string>
#include <SFML/Graphics.hpp>
#include "GameObjectSharer.h"
#include <iostream>


class GraphicsComponent : public Component
{
    private:
        bool m_Enabled = false;

    public:
        virtual void draw(sf::RenderWindow& window, std::shared_ptr<TransformComponent> t) = 0;
        virtual void initializeGraphics(std::string bitmapName, sf::Vector2f objectSize) = 0;

        // From Component interface
        ComponentType getType() const override
        {
            return ComponentType::Graphics;
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