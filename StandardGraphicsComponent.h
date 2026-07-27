#pragma once
#include "Component.h"
#include "GraphicsComponent.h"
#include <string>

class Component;

class StandardGraphicsComponent : public GraphicsComponent
{
    private:
        sf::Sprite m_Sprite;
        string m_SpecificType = "standard";

    public:
        // From Component interface
        // override virtual functions
        string getSpecificType() override
        {
            return m_SpecificType;
        }

        void start(GameObjectSharer*, GameObject*) override {}

        // From GraphicsComponent
        // override virtual functions
        void draw(RenderWindow& window, shared_ptr<TransformComponent> t) override;
        void initializeGraphics(string bitmapName, Vector2f objectSize) override;
};