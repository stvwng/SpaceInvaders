#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Component.h"
#include "GraphicsComponent.h"
#include "GameObjectSharer.h"
#include "UpdateComponent.h"
#include "ComponentType.h"

class GameObject
{
    private:
        std::vector<std::shared_ptr<Component>> m_Components;

        ObjectTag m_Tag = ObjectTag::Unknown;
        // Original spelling from the level file, kept for diagnostics.
        std::string m_TagName;
        bool m_Active = false;
        int m_NumberUpdateComponents = 0;
        bool m_HasUpdateComponent = false;
        int m_FirstUpdateComponentLocation = -1;
        int m_GraphicsComponentLocation = -1;
        bool m_HasGraphicsComponent = false;
        int m_TransformComponentLocation = -1;
        int m_NumberRectColliderComponents = 0;
        int m_FirstRectColliderComponentLocation = -1;
        bool m_HasCollider = false;

    public:
        void update(float dt);
        void draw(sf::RenderWindow& window);
        void addComponent(std::shared_ptr<Component> component);

        void setActive();
        void setInactive();
        bool isActive() const;
        void setTag(std::string tag);
        ObjectTag getTag() const;
        const std::string& getTagName() const;

        void start(GameObjectSharer* gos);

        std::shared_ptr<Component> getComponentByTypeAndSpecificType(ComponentType type, ComponentSpecificType specificType);

        sf::FloatRect& getEncompassingRectCollider();
        bool hasCollider() const;
        bool hasUpdateComponent() const;
        std::string getEncompassingRectColliderTag();

        std::shared_ptr<GraphicsComponent> getGraphicsComponent();
        std::shared_ptr<TransformComponent> getTransformComponent();
        std::shared_ptr<UpdateComponent> getFirstUpdateComponent();
};