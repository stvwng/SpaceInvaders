#pragma once
#include <random>

// One random engine for the whole game.
//
// The original code called srand() at runtime -- inside InvaderUpdateComponent
// ::update() on every shot, and inside BulletUpdateComponent::spawnForInvader()
// on every spawn. Re-seeding a generator mid-sequence throws away the
// randomness it is trying to produce: srand(time(0)) makes every call within
// the same second return the same value, and seeding with an object's index
// makes every object at that index produce an identical sequence.
//
// Seed once, then keep drawing.
class Random
{
    public:
        static float inRange(float low, float high);
        static int inRange(int low, int high);

    private:
        static std::mt19937& engine();
};
