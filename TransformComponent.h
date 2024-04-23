#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>

using namespace sf;

class Component;

class TransformComponent : public GraphicsComponent
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
        string getType()
        {
            return m_Type;
        }

        string getSpecificType()
        {
            return m_Type; // only one type of Transform
        }

        void disableComponent(){}
        void enableComponent(){}
        bool enabled()
        {
            return false;
        }
        void start(GameObjectSharer* gos, GameObject* self){}
};