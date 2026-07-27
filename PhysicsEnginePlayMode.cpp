#include "PhysicsEnginePlayMode.h"
#include "DevelopState.h"
#include <iostream>
#include "WorldState.h"
#include "InvaderUpdateComponent.h"
#include "BulletUpdateComponent.h"

using namespace std;
using namespace sf;

void PhysicsEnginePlayMode::detectInvaderCollisions(
    vector<GameObject>& objects,
    const vector<int>& bulletPositions
)
{
    if (bulletPositions.empty())
    {
        // A level with no bullet objects. advance(it, bulletPositions[0]) on an
        // empty vector used to read past the end here.
        return;
    }

    const Vector2f offScreen(-1, -1);

    // Gather the bullets that can actually hit something, once per frame,
    // rather than re-deciding it for every (invader, bullet) pair.
    //
    // The inner loop used to walk from the first bullet to the end of the whole
    // object vector and, for each step, call getFirstUpdateComponent() -- which
    // returns a shared_ptr **by value**, so every pair cost an atomic reference
    // count increment and decrement. With 45 invaders and 14 bullets that was
    // 630 atomic operations per frame to answer a question whose answer is the
    // same for all 45 invaders. Typically only one or two bullets are in flight.
    //
    // m_LiveBullets is a member so its capacity survives between frames and the
    // gather does not allocate.
    m_LiveBullets.clear();
    for (const int bulletIndex : bulletPositions)
    {
        GameObject& bullet = objects[static_cast<size_t>(bulletIndex)];
        const BulletUpdateComponent& update = *static_pointer_cast<BulletUpdateComponent>(
            bullet.getFirstUpdateComponent()
        );

        // Only a bullet in flight, owned by the player, can kill an invader.
        // The original code tested neither, so a parked bullet at (-1,-1) could
        // score a hit on an invader that had drifted to a negative x.
        if (update.m_IsSpawned && update.m_BelongsToPlayer)
        {
            m_LiveBullets.push_back(&bullet);
        }
    }

    if (m_LiveBullets.empty())
    {
        return;
    }

    for (auto invaderIt = objects.begin(); invaderIt != objects.end(); ++invaderIt)
    {
        if (!invaderIt->isActive() || invaderIt->getTag() != ObjectTag::Invader)
        {
            continue;
        }

        for (GameObject* bulletObject : m_LiveBullets)
        {
            GameObject& bulletRef = *bulletObject;

            if (!invaderIt->getEncompassingRectCollider().intersects(
                    bulletRef.getEncompassingRectCollider()))
            {
                continue;
            }

            auto bulletUpdate = static_pointer_cast<BulletUpdateComponent>(
                bulletRef.getFirstUpdateComponent()
            );

            m_SoundPlayer->playInvaderExplode();
            invaderIt->getTransformComponent()->getLocation() = offScreen;
            bulletRef.getTransformComponent()->getLocation() = offScreen;
            bulletUpdate->deSpawn();

            WorldState::SCORE++;
            WorldState::NUM_INVADERS--;
            invaderIt->setInactive();

            // One bullet, one invader. Without this break the same invader
            // could be killed several times in a single frame by overlapping
            // bullets, driving NUM_INVADERS negative and triggering a
            // premature wave advance.
            break;
        }
    }
}

void PhysicsEnginePlayMode::detectPlayerCollisionsAndInvaderDirection(
    vector<GameObject>& objects,
    const vector<int>&
)
{
    const Vector2f offScreen(-1, -1);
    const FloatRect playerCollider = m_Player->getEncompassingRectCollider();

    for (auto it = objects.begin(); it != objects.end(); ++it)
    {
        // Everything *except* the player. This condition read `== "Player"`,
        // which made the entire body below unreachable for bullets and
        // invaders: the player could never be hit, so the game had no fail
        // state, and the invaders never dropped down and reversed.
        if (!it->isActive() || !it->hasCollider() || it->getTag() == ObjectTag::Player)
        {
            continue;
        }

        const ObjectTag currentTag = it->getTag();
        shared_ptr<TransformComponent> currentTransform = it->getTransformComponent();
        const Vector2f currentLocation = currentTransform->getLocation();
        const Vector2f currentSize = currentTransform->getSize();

        // --- Collisions with the player ---
        if (it->getEncompassingRectCollider().intersects(playerCollider))
        {
            if (currentTag == ObjectTag::Bullet)
            {
                auto bulletUpdate = static_pointer_cast<BulletUpdateComponent>(
                    it->getFirstUpdateComponent()
                );

                // The player's own bullets leave from inside the player's
                // collider, so they must not count as a hit on the player.
                if (bulletUpdate->m_IsSpawned && !bulletUpdate->m_BelongsToPlayer)
                {
                    m_SoundPlayer->playPlayerExplode();
                    WorldState::LIVES--;
                    currentTransform->getLocation() = offScreen;
                    bulletUpdate->deSpawn();
                }
            }
            else if (currentTag == ObjectTag::Invader)
            {
                m_SoundPlayer->playPlayerExplode();
                m_SoundPlayer->playInvaderExplode();
                WorldState::LIVES--;
                WorldState::SCORE++;
                // The invader is deactivated here, so it must also come off the
                // count -- otherwise the wave can never be cleared.
                WorldState::NUM_INVADERS--;
                currentTransform->getLocation() = offScreen;
                it->setInactive();
            }
        }

        // --- Direction and descent of the invaders ---
        if (currentTag == ObjectTag::Invader)
        {
            if (!m_NeedToDropDownAndReverse && !m_InvaderHitWallThisFrame)
            {
                if (currentLocation.x >= WorldState::WORLD_WIDTH - currentSize.x)
                {
                    // Past the furthest right position, travelling right.
                    if (static_pointer_cast<InvaderUpdateComponent>(
                            it->getFirstUpdateComponent())->isMovingRight())
                    {
                        m_InvaderHitWallThisFrame = true;
                    }
                }
                else if (currentLocation.x < 0)
                {
                    // Past the furthest left position, travelling left.
                    if (!static_pointer_cast<InvaderUpdateComponent>(
                            it->getFirstUpdateComponent())->isMovingRight())
                    {
                        m_InvaderHitWallThisFrame = true;
                    }
                }
            }
            else if (m_NeedToDropDownAndReverse)
            {
                if (it->hasUpdateComponent())
                {
                    static_pointer_cast<InvaderUpdateComponent>(
                        it->getFirstUpdateComponent())->dropDownAndReverse();
                }
            }
        }
    }
}

void PhysicsEnginePlayMode::handleInvaderDirection()
{
    // Two-frame handshake. Frame N: some invader reports hitting a wall.
    // Frame N+1: every invader drops down and reverses exactly once, because
    // m_InvaderHitWallThisFrame is already cleared and so no new hit is
    // recorded while the whole formation is still outside the bounds.
    if (m_InvaderHitWallThisFrame)
    {
        m_NeedToDropDownAndReverse = true;
        m_InvaderHitWallThisFrame = false;
    }
    else
    {
        m_NeedToDropDownAndReverse = false;
    }
}

void PhysicsEnginePlayMode::initialize(GameObjectSharer& gos, SoundPlayer& soundPlayer)
{
    m_Player = &gos.findFirstObjectWithTag("Player");
    m_SoundPlayer = &soundPlayer;

    // A new wave reuses this engine, so the handshake state must not carry
    // over from the previous one.
    m_InvaderHitWallThisFrame = false;
    m_NeedToDropDownAndReverse = false;
}

void PhysicsEnginePlayMode::detectCollisions(
    vector<GameObject>& objects,
    const vector<int>& bulletPositions
)
{
    detectInvaderCollisions(objects, bulletPositions);
    detectPlayerCollisionsAndInvaderDirection(objects, bulletPositions);
    handleInvaderDirection();
}
