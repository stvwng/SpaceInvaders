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
        string getSpecificType()
        {
            return m_SpecificType;
        }

        void start(GameObjectSharer* gos, GameObject* self){}

        // From GraphicsComponent
        // override virtual functions
        void draw(RenderWindow& window, shared_ptr<TransformComponent> t) override;
        void intializeGraphics(string bitmapName, Vector2f objectSize) override;
};