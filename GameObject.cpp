#include "GameObject.h"
#include "DevelopState.h"
#include "UpdateComponent.h"
#include "RectColliderComponent.h"
#include <iostream>

void GameObject::update(float fps)
{
    if (m_Active && m_HasUpdateComponent)
    {
        for (int i = m_FirstUpdateComponentLocation; i < m_FirstUpdateComponentLocation + m_NumberUpdateComponents; i++)
        {
            shared_ptr<UpdateComponent> tempUpdate = static_pointer_cast<UpdateComponent>(m_Components[i]);

            if (tempUpdate->enabled())
            {
                tempUpdate->update(fps);
            }
        }
    }
}

void GameObject::draw(RenderWindow& window)
{
    if (m_Active && m_HasGraphicsComponent)
    {
        if (m_Components[m_GraphicsComponentLocation]->enabled())
        {
            getGraphicsComponent()->draw(window, getTransformComponent());
        }
    }
}

shared_ptr<GraphicsComponent> GameObject::getGraphicsComponent()
{
    return static_pointer_cast<GraphicsComponent>(m_Components[m_GraphicsComponentLocation]);
}

shared_ptr<TransformComponent> GameObject::getTransformComponent()
{
    return static_pointer_cast<TransformComponent>(m_Components[m_TransformComponentLocation]);
}

void GameObject::addComponent(shared_ptr<Component> component)
{
    m_Components.push_back(component);
    component->enableComponent();

    if (component->getType() == "update")
    {
        m_HasUpdateComponent = true;
        m_NumberUpdateComponents++;
        if (m_NumberUpdateComponents == 1)
        {
            m_FirstUpdateComponentLocation = m_Components.size() - 1;
        }
    }
    else if (component->getType() == "graphics")
    {
        m_HasGraphicsComponent = true;
        m_GraphicsComponentLocation = m_Components.size() - 1;
    }
    else if (component->getType() == "transform")
    {
        m_TransformComponentLocation = m_Components.size() - 1;
    }
    else if (component->getType() == "collider" && component->getSpecificType == "rect")
    {
        m_HasCollider = true;
        m_NumberRectColliderComponents++;
        if (m_NumberRectColliderComponents == 1)
        {
            m_FirstRectColliderComponentLocation = m_Components.size() - 1;
        }
    }
}

void GameObject::setActive()
{
    m_Active = true;
}

void GameObject::setInactive()
{
    m_Active = false;
}

bool GameObject::isActive()
{
    return m_Active;
}

void GameObject::setTag(string tag)
{
    m_Tag = "" + tag;
}

std::string GameObject::getTag()
{
    return m_Tag;
}

void GameObject::start(GameObjectSharer* gos)
{
    auto it = m_Components.begin();
    auto end = m_Components.end();
    for (it; it != end; ++it)
    {
        (*it)->start(gos, this);
    }
}

shared_ptr<Component> GameObject::getComponentByTypeAndSpecificType(string type, string specificType)
{
    auto it = m_Components.begin();
    auto end = m_Components.end();
    for (it; it != end; ++it)
    {
        if ((*it)->getType() == type)
        {
            if ((*it)->getSpecificType() == specificType)
            {
                return (*it);
            }
        }
    }

    #ifdef debuggingErrors
    cout << "GameObject.cpp::getComponentByTypeAndSpecificType-"
         << "COMPONENT NOT FOUND ERROR"
         << end;
    #endif

    return m_Components[0];
}

FloatRect& GameObject::getEncompassingRectCollider()
{
    if (m_HasCollider)
    {
        return (static_pointer_cast<RectColliderComponent>(
            m_Components[m_FirstRectColliderComponentLocation]
        ))->getColliderRectF();
    }
}

string GameObject::getEncompassingColliderTag()
{
    return (static_pointer_cast<RectColliderComponent>(m_Components[m_FirstRectColliderComponentLocation]))->getColliderTag();
}

shared_ptr<UpdateComponent> GameObject::getFirstUpdateComponent()
{
    return static_pointer_cast<UpdateComponent>(m_Components[m_FirstUpdateComponentLocation]);
}

bool GameObject::hasCollider()
{
    return m_HasCollider;
}

bool GameObject::hasUpdateComponent()
{
    return m_HasUpdateComponent;
}