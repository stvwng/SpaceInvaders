#include "BulletUpdateComponent.h"
#include "WorldState.h"
#include "Random.h"

using namespace std;
using namespace sf;

void BulletUpdateComponent::spawnForPlayer(Vector2f spawnPosition)
{
    m_MovingUp = true;
    m_BelongsToPlayer = true;
    m_IsSpawned = true;

    m_TC->getLocation().x = spawnPosition.x;
    m_TC->getLocation().y = spawnPosition.y - m_TC->getSize().y;

    // Update collider
    m_RCC->setOrMoveCollider(
        m_TC->getLocation().x,
        m_TC->getLocation().y,
        m_TC->getSize().x,
        m_TC->getSize().y
    );
}

void BulletUpdateComponent::spawnForInvader(Vector2f spawnPosition)
{
    m_MovingUp = false;
    m_BelongsToPlayer = false;
    m_IsSpawned = true;

    // srand((int)time(0)) ran on every spawn, so every invader bullet fired
    // within the same second travelled at exactly the same speed, and the
    // re-seeding discarded the generator's state each time.
    m_AlienBulletSpeedModifier =
        Random::inRange(0, m_ModifierRandomComponent - 1) + m_MinimumAdditionalModifier;

    m_TC->getLocation().x = spawnPosition.x;
    m_TC->getLocation().y = spawnPosition.y + m_TC->getSize().y;

    // Update collider
    m_RCC->setOrMoveCollider(
        m_TC->getLocation().x,
        m_TC->getLocation().y,
        m_TC->getSize().x,
        m_TC->getSize().y
    );
}

void BulletUpdateComponent::deSpawn()
{
    m_IsSpawned = false;
}

bool BulletUpdateComponent::isMovingUp()
{
    return m_MovingUp;
}

void BulletUpdateComponent::update(float dt)
{
    if (m_IsSpawned)
    {
        if (m_MovingUp)
        {
            m_TC->getLocation().y -= m_Speed * dt;
        }
        else
        {
            m_TC->getLocation().y += m_Speed / m_AlienBulletSpeedModifier * dt;
        }

        if (m_TC->getLocation().y > WorldState::WORLD_HEIGHT || m_TC->getLocation().y < -2)
        {
            deSpawn();
        }

        // Update the collider
        m_RCC->setOrMoveCollider(
            m_TC->getLocation().x,
            m_TC->getLocation().y,
            m_TC->getSize().x,
            m_TC->getSize().y
        );       
    }
}