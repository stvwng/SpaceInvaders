#pragma once
#include "ColliderComponent.h"
#include <SFML/Graphics.hpp>


class RectColliderComponent : public ColliderComponent
{
    private:
        sf::FloatRect m_Collider;
        std::string m_Tag = "";

    public:
        RectColliderComponent(std::string name);
        std::string getColliderTag();
        void setOrMoveCollider(float x, float y, float width, float height);

        sf::FloatRect& getColliderRectF();

        // From Component interface
        // override virtual functions
        ComponentSpecificType getSpecificType() const override
        {
            return ComponentSpecificType::Rect;
        }

        void start(GameObjectSharer*, GameObject*) override {}
};