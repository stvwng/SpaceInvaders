#include "InvaderUpdateComponent.h"
#include "BulletSpawner.h"
#include "WorldState.h"
#include "Random.h"

void InvaderUpdateComponent::update(float dt)
{
    if (m_MovingRight)
    {
        m_TC->getLocation().x += m_Speed * dt;
    }
    else
    {
        m_TC->getLocation().x -= m_Speed * dt;
    }

    // Update the collider
    m_RCC->setOrMoveCollider(
        m_TC->getLocation().x,
        m_TC->getLocation().y,
        m_TC->getSize().x,
        m_TC->getSize().y
    );

    m_TimeSinceLastShot += dt;

    // Is the middle of the invader above the player +- 1 world units?
    if ((m_TC->getLocation().x + (m_TC->getSize().x / 2)) > (m_PlayerTC->getLocation().x - m_AccuracyModifier) &&
        (m_TC->getLocation().x + (m_TC->getSize().x / 2)) < (m_PlayerTC->getLocation().x + (m_PlayerTC->getSize().x + m_AccuracyModifier))
    )
    {
        // Has invader waited long enough since last shot?
        if (m_TimeSinceLastShot > m_TimeBetweenShots)
        {
            m_SoundPlayer->playShoot();
            Vector2f spawnLocation;
            spawnLocation.x = m_TC->getLocation().x + m_TC->getSize().x / 2;
            spawnLocation.y = m_TC->getLocation().y + m_TC->getSize().y;

            m_BulletSpawner->spawnBullet(spawnLocation, false);

            // This was integer division: ((rand() % 10) + 1) / WAVE_NUMBER.
            // From wave 11 onwards the result is 0, so the invader fires on
            // every single frame. Float division with a floor keeps later
            // waves fast without turning them into a solid wall of bullets.
            const int waveNumber = WorldState::WAVE_NUMBER < 1 ? 1 : WorldState::WAVE_NUMBER;
            m_TimeBetweenShots = Random::inRange(1.f, 10.f) / static_cast<float>(waveNumber);
            if (m_TimeBetweenShots < MIN_TIME_BETWEEN_SHOTS)
            {
                m_TimeBetweenShots = MIN_TIME_BETWEEN_SHOTS;
            }

            m_TimeSinceLastShot = 0.f;
        }
    }
}

void InvaderUpdateComponent::dropDownAndReverse()
{
    m_MovingRight = !m_MovingRight;
    m_TC->getLocation().y += m_TC->getSize().y;
    m_Speed += (WorldState::WAVE_NUMBER) + (WorldState::NUM_INVADERS_AT_START - WorldState::NUM_INVADERS) * m_SpeedModifier;
}

bool InvaderUpdateComponent::isMovingRight()
{
    return m_MovingRight;
}

void InvaderUpdateComponent::initializeBulletSpawner(BulletSpawner* bulletSpawner, int indexInLevel, SoundPlayer& soundPlayer)
{
    m_BulletSpawner = bulletSpawner;
    m_SoundPlayer = &soundPlayer;

    // indexInLevel is this invader's position in the GameObject vector. It used
    // to be fed to srand() and then added straight onto the first shot delay:
    //     m_TimeBetweenShots = rand() % 15 + randSeed;
    // With 45 invaders in the level that meant the back rows waited up to 59
    // seconds before firing a single shot. It is now only a small stagger, so
    // the formation does not open fire in unison.
    const float stagger = static_cast<float>(indexInLevel % 5);
    m_TimeBetweenShots = Random::inRange(1.f, 8.f) + stagger;
    m_TimeSinceLastShot = 0.f;

    // How far horizontally from the player this invader will still take a
    // shot, in world units.
    m_AccuracyModifier = Random::inRange(0.f, 11.f);
}