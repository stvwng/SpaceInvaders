#pragma once
#include "GameObjectSharer.h"
#include "SoundPlayer.h"
#include "GameObject.h"
#include <vector>
#include <memory>

class PhysicsEnginePlayMode
{
    private:
        // Permanent reference to the Player so it need not be found every frame.
        // This points into the LevelManager's vector<GameObject>, so it is only
        // valid while that vector is not reallocated. initialize() re-resolves
        // it after every level load, which is what keeps it safe.
        GameObject* m_Player = nullptr;

        // Set by initialize(); owned by GameEngine.
        SoundPlayer* m_SoundPlayer = nullptr;

        bool m_InvaderHitWallThisFrame = false;
        bool m_NeedToDropDownAndReverse = false;

        // Sticky once set: the invaders have arrived at the player's row and the
        // game is lost. Cleared only by initialize(), i.e. by a level load.
        bool m_InvadersReachedPlayer = false;

        // Scratch space for detectInvaderCollisions, held as a member purely so
        // its capacity survives between frames and the per-frame gather does
        // not allocate. Contents are meaningless outside that function.
        std::vector<GameObject*> m_LiveBullets;

        void detectInvaderCollisions(
            std::vector<GameObject>& objects,
            const std::vector<int>& bulletPositions
        );

        void detectPlayerCollisionsAndInvaderDirection(
            std::vector<GameObject>& objects,
            const std::vector<int>& bulletPositions
        );

        void handleInvaderDirection();

    public:
        void initialize(GameObjectSharer& gos, SoundPlayer& soundPlayer);
        void detectCollisions(
            std::vector<GameObject>& objects,
            const std::vector<int>& bulletPositions
        );

        bool invadersReachedPlayer() const;
};
