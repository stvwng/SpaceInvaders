#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>

using namespace sf;

// A transform is not a kind of graphics. This originally derived from
// GraphicsComponent, which made it abstract (it never implemented draw() or
// initializeGraphics()) and created a circular include with GraphicsComponent.h.
class TransformComponent : public Component
{
    private:
        const string m_Type = "transform";
        Vector2f m_Location;
        float m_Height;
        float m_Width;

    public:
        TransformComponent(float width, float height, Vector2f location);
        Vector2f& getLocation();
        Vector2f getSize();

        // virtual functions to override from Component interface
        string getType() override
        {
            return m_Type;
        }

        string getSpecificType() override
        {
            return m_Type; // only one type of Transform
        }

        void disableComponent() override {}
        void enableComponent() override {}
        bool enabled() override
        {
            return false;
        }
        void start(GameObjectSharer*, GameObject*) override {}
};