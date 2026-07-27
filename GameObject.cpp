#include "GameObject.h"
#include "DevelopState.h"
#include "UpdateComponent.h"
#include "RectColliderComponent.h"
#include <iostream>
#include <stdexcept>

void GameObject::update(float dt)
{
    if (m_Active && m_HasUpdateComponent)
    {
        for (int i = m_FirstUpdateComponentLocation; i < m_FirstUpdateComponentLocation + m_NumberUpdateComponents; i++)
        {
            shared_ptr<UpdateComponent> tempUpdate = static_pointer_cast<UpdateComponent>(m_Components[i]);

            if (tempUpdate->enabled())
            {
                tempUpdate->update(dt);
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

// These three accessors index m_Components by a cached position that stays -1
// until the matching component is added. Indexing a vector with -1 is undefined
// behaviour, and it is genuinely reachable: a level file naming a component the
// factory does not recognise produces an object with none of them. Fail with a
// message that names the object instead of reading 16 bytes off the front of
// the array.
shared_ptr<GraphicsComponent> GameObject::getGraphicsComponent()
{
    if (m_GraphicsComponentLocation < 0)
    {
        throw std::runtime_error(
            std::string("GameObject::getGraphicsComponent - object tagged \"") + m_TagName + "\" has no graphics component"
        );
    }
    return static_pointer_cast<GraphicsComponent>(m_Components[m_GraphicsComponentLocation]);
}

shared_ptr<TransformComponent> GameObject::getTransformComponent()
{
    if (m_TransformComponentLocation < 0)
    {
        throw std::runtime_error(
            std::string("GameObject::getTransformComponent - object tagged \"") + m_TagName + "\" has no transform component"
        );
    }
    return static_pointer_cast<TransformComponent>(m_Components[m_TransformComponentLocation]);
}

void GameObject::addComponent(shared_ptr<Component> component)
{
    m_Components.push_back(component);
    component->enableComponent();

    if (component->getType() == ComponentType::Update)
    {
        m_HasUpdateComponent = true;
        m_NumberUpdateComponents++;
        if (m_NumberUpdateComponents == 1)
        {
            m_FirstUpdateComponentLocation = m_Components.size() - 1;
        }
    }
    else if (component->getType() == ComponentType::Graphics)
    {
        m_HasGraphicsComponent = true;
        m_GraphicsComponentLocation = m_Components.size() - 1;
    }
    else if (component->getType() == ComponentType::Transform)
    {
        m_TransformComponentLocation = m_Components.size() - 1;
    }
    else if (component->getType() == ComponentType::Collider && component->getSpecificType() == ComponentSpecificType::Rect)
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

bool GameObject::isActive() const
{
    return m_Active;
}

void GameObject::setTag(string tag)
{
    m_TagName = tag;
    m_Tag = toObjectTag(tag);
}

ObjectTag GameObject::getTag() const
{
    return m_Tag;
}

const std::string& GameObject::getTagName() const
{
    return m_TagName;
}

void GameObject::start(GameObjectSharer* gos)
{
    auto it = m_Components.begin();
    auto end = m_Components.end();
    for (; it != end; ++it)
    {
        (*it)->start(gos, this);
    }
}

shared_ptr<Component> GameObject::getComponentByTypeAndSpecificType(ComponentType type, ComponentSpecificType specificType)
{
    auto it = m_Components.begin();
    auto end = m_Components.end();
    for (; it != end; ++it)
    {
        if ((*it)->getType() == type)
        {
            if ((*it)->getSpecificType() == specificType)
            {
                return (*it);
            }
        }
    }

    // This used to return m_Components[0]. Every caller immediately
    // static_pointer_cast's the result to a concrete component type, so
    // returning the wrong component is undefined behaviour that shows up later
    // as inexplicable movement or a crash in an unrelated system.
    throw std::runtime_error(
        std::string("GameObject::getComponentByTypeAndSpecificType - object tagged \"") + m_TagName +
        "\" has no component of type \"" + toString(type) + "\" / \"" + toString(specificType) + "\""
    );
}

FloatRect& GameObject::getEncompassingRectCollider()
{
    if (m_HasCollider)
    {
        return (static_pointer_cast<RectColliderComponent>(
            m_Components[m_FirstRectColliderComponentLocation]
        ))->getColliderRectF();
    }

    // Previously this fell off the end of the function, which is undefined
    // behaviour: the caller got a reference to whatever happened to be in the
    // return register. Callers are expected to gate on hasCollider(); this
    // degenerate rect intersects nothing, so a missed check misbehaves
    // visibly instead of corrupting memory.
    static FloatRect noCollider(0.f, 0.f, 0.f, 0.f);
    return noCollider;
}

string GameObject::getEncompassingRectColliderTag()
{
    return (static_pointer_cast<RectColliderComponent>(m_Components[m_FirstRectColliderComponentLocation]))->getColliderTag();
}

shared_ptr<UpdateComponent> GameObject::getFirstUpdateComponent()
{
    if (m_FirstUpdateComponentLocation < 0)
    {
        throw std::runtime_error(
            std::string("GameObject::getFirstUpdateComponent - object tagged \"") + m_TagName + "\" has no update component"
        );
    }
    return static_pointer_cast<UpdateComponent>(m_Components[m_FirstUpdateComponentLocation]);
}

bool GameObject::hasCollider() const
{
    return m_HasCollider;
}

bool GameObject::hasUpdateComponent() const
{
    return m_HasUpdateComponent;
}