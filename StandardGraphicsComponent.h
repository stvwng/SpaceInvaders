#pragma once
#include "Component.h"
#include "GraphicsComponent.h"
#include <string>

class Component;

class StandardGraphicsComponent : public GraphicsComponent
{
    private:
        sf::Sprite m_Sprite;

    public:
        // From Component interface
        // override virtual functions
        ComponentSpecificType getSpecificType() const override
        {
            return ComponentSpecificType::Standard;
        }

        void start(GameObjectSharer*, GameObject*) override {}

        // From GraphicsComponent
        // override virtual functions
        void draw(sf::RenderWindow& window, std::shared_ptr<TransformComponent> t) override;
        void initializeGraphics(std::string bitmapName, sf::Vector2f objectSize) override;
};