#pragma once
#include "GameObjectSharer.h"
#include "PlayerUpdateComponent.h"
#include <memory>

class PhysicsEnginePlayMode
{
    private:
        shared_ptr<PlayerUpdateComponent> m_PUC;

        GameObject* m_Player; // permanent reference to Player so we don't need to keep finding it every frame
        bool m_InvaderHitWallThisFrame = false;
        bool m_InvaderHitWallPreviousFrame = false;
        bool m_NeedToDropDownAndReverse = false;
        bool m_CompleteDropDownAndReverse = false;

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
        void initialize(GameObjectSharer& gos);
        void detectCollisions(
            vector<GameObject>& objects,
            const vector<int>& bulletPositions
        );
};