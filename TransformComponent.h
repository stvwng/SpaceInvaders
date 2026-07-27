#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>


// A transform is not a kind of graphics. This originally derived from
// GraphicsComponent, which made it abstract (it never implemented draw() or
// initializeGraphics()) and created a circular include with GraphicsComponent.h.
class TransformComponent : public Component
{
    private:
                sf::Vector2f m_Location;
        float m_Height;
        float m_Width;

    public:
        TransformComponent(float width, float height, sf::Vector2f location);
        sf::Vector2f& getLocation();
        sf::Vector2f getSize();

        // virtual functions to override from Component interface
        ComponentType getType() const override
        {
            return ComponentType::Transform;
        }

        ComponentSpecificType getSpecificType() const override
        {
            return ComponentSpecificType::Transform; // only one kind of transform
        }

        void disableComponent() override {}
        void enableComponent() override {}
        bool enabled() const override
        {
            return false;
        }
        void start(GameObjectSharer*, GameObject*) override {}
};