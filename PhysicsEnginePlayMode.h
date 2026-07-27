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

        void detectInvaderCollisions(
            vector<GameObject>& objects,
            const vector<int>& bulletPositions
        );

        void detectPlayerCollisionsAndInvaderDirection(
            vector<GameObject>& objects,
            const vector<int>& bulletPositions
        );

        void handleInvaderDirection();

    public:
        void initialize(GameObjectSharer& gos, SoundPlayer& soundPlayer);
        void detectCollisions(
            vector<GameObject>& objects,
            const vector<int>& bulletPositions
        );
};
