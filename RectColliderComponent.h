#pragma once
#include "ColliderComponent.h"
#include <SFML/Graphics.hpp>

using namespace sf;

class RectColliderComponent : public ColliderComponent
{
    private:
        FloatRect m_Collider;
        string m_Tag = "";

    public:
        RectColliderComponent(string name);
        string getColliderTag();
        void setOrMoveCollider(float x, float y, float width, float height);

        FloatRect& getColliderRectF();

        // From Component interface
        // override virtual functions
        ComponentSpecificType getSpecificType() const override
        {
            return ComponentSpecificType::Rect;
        }

        void start(GameObjectSharer*, GameObject*) override {}
};