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
        vector<shared_ptr<Component>> m_Components;

        ObjectTag m_Tag = ObjectTag::Unknown;
        // Original spelling from the level file, kept for diagnostics.
        string m_TagName;
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
        void draw(RenderWindow& window);
        void addComponent(shared_ptr<Component> component);

        void setActive();
        void setInactive();
        bool isActive() const;
        void setTag(string tag);
        ObjectTag getTag() const;
        const string& getTagName() const;

        void start(GameObjectSharer* gos);

        shared_ptr<Component> getComponentByTypeAndSpecificType(ComponentType type, ComponentSpecificType specificType);

        FloatRect& getEncompassingRectCollider();
        bool hasCollider() const;
        bool hasUpdateComponent() const;
        string getEncompassingRectColliderTag();

        shared_ptr<GraphicsComponent> getGraphicsComponent();
        shared_ptr<TransformComponent> getTransformComponent();
        shared_ptr<UpdateComponent> getFirstUpdateComponent();
};