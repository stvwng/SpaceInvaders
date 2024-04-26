#include "PhysicsEnginePlayMode.h"
#include "DevelopState.h"
#include <iostream>
#include "SoundEngine.h"
#include "WorldState.h"
#include "InvaderUpdateComponent.h"
#include "BulletUpdateComponent.h"

void PhysicsEnginePlayMode::detectInvaderCollisions(
    vector<GameObject>& objects,
    const vector<int>& bulletPositions
)
{
    Vector2f offScreen(-1, -1);

    auto invaderIt = objects.begin();
    auto invaderEnd = objects.end();
    for (invaderIt; invaderIt != invaderEnd; ++invaderIt)
    {
        if (invaderIt->isActive() && invaderIt->getTag() == "invader")
        {
            auto bulletIt = objects.begin();
            // Jump to first bullet
            advance(bulletIt, bulletPositions[0]);
            auto bulletEnd = objects.end();
            for (bulletIt; bulletIt != bulletEnd; ++bulletIt)
            {
                if (
                    invaderIt->getEncompassingRectCollider().intersects(bulletIt->getEncompassingRectCollider()) &&
                    bulletIt->getTag() == "bullet" &&
                    static_pointer_cast<BulletUpdateComponent>(bulletIt->getFirstUpdateComponent())->m_BelongsToPlayer
                )
                {
                    SoundEngine::playInvaderExplode();
                    invaderIt->getTransformComponent()->getLocation() = offScreen;
                    bulletIt->getTransformComponent()->getLocation() = offScreen;

                    WorldState::SCORE++;
                    WorldState::NUM_INVADERS--;
                    invaderIt->setInactive();
                }
            }
        }
    }
}

void PhysicsEnginePlayMode::detectPlayerCollisionsAndInvaderDirection(
    vector<GameObject>& objects,
    const vector<int>& bulletPositions
)
{
    Vector2f offScreen(-1, -1);
    FloatRect playerCollider = m_Player->getEncompassingRectCollider();
    shared_ptr<TransformComponent> playerTransform = m_Player->getTransformComponent();
    Vector2f playerLocation = playerTransform->getLocation();

    auto it3 = objects.begin();
    auto end3 = objects.end();
    for (it3; it3 != end3; ++it3)
    {
        if (
            it3->isActive() &&
            it3->hasCollider() &&
            it3->getTag() == "Player"
        )
        {
            // Get reference to all parts of the current game object that may be needed
            FloatRect currentCollider = it3->getEncompassingRectCollider();

            // Detect collisions between objects and player
            if (currentCollider.intersects(playerCollider))
            {
                if (it3->getTag() == "bullet")
                {
                    SoundEngine::playPlayerExplode();
                    WorldState::LIVES--;
                    it3->getTransformComponent()->getLocation() = offScreen;
                }

                if (it3->getTag() == "invader")
                {
                    SoundEngine::playPlayerExplode();
                    SoundEngine::playInvaderExplode();
                    WorldState::LIVES--;
                    it3->getTransformComponent()->getLocation() = offScreen;
                    WorldState::SCORE++;
                    it3->setInactive();
                }
            }

            shared_ptr<TransformComponent> currentTransform = it3->getTransformComponent();
            Vector2f currentLocation = currentTransform->getLocation();
            string currentTag = it3->getTag();
            Vector2f currentSize = currentTransform->getSize();

            // Handle direction and descent of invaders
            if (currentTag == "invader")
            {
                if (!m_NeedToDropDownAndReverse && !m_InvaderHitWallThisFrame)
                {
                    // No need to drop down and reverse from previous frame or any hits this frame
                    if (currentLocation.x >= WorldState::WORLD_WIDTH - currentSize.x)
                    {
                        // Invader has passed its furthest right position
                        if (static_pointer_cast<InvaderUpdateComponent>(it3->getFirstUpdateComponent())->isMovingRight())
                        {
                            // Invader is travelling right; set flag that invader has collided
                            m_InvaderHitWallThisFrame = true;
                        }
                    }
                    else if (currentLocation.x < 0)
                    {
                        // Invader is past its furthest left position
                        if (!static_pointer_cast<InvaderUpdateComponent>(it3->getFirstUpdateComponent())->isMovingRight())
                        {
                            // Invader is travelling left; set flag that invader has collided
                            m_InvaderHitWallThisFrame = true;
                        }
                    }
                }
                else if (m_NeedToDropDownAndReverse && !m_InvaderHitWallPreviousFrame)
                {
                    // Drop down and reverse has been set
                    if (it3->hasUpdateComponent())
                    {
                        // Drop down and reverse
                        static_pointer_cast<InvaderUpdateComponent>(it3->getFirstUpdateComponent())->dropDownAndReverse();
                    }
                }
            }
        }
    }
}

void PhysicsEnginePlayMode::handleInvaderDirection()
{
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

void PhysicsEnginePlayMode::initialize(GameObjectSharer& gos)
{
    m_PUC = static_pointer_cast<PlayerUpdateComponent>(
        gos.findFirstObjectWithTag("Player").getComponentByTypeAndSpecificType("update", "player")
    );
    m_Player = &gos.findFirstObjectWithTag("Player");
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