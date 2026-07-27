// Headless timing harness for the simulation.
//
// Loads the real world/level1 and runs the per-frame work GameScreen does --
// every GameObject::update, then PhysicsEnginePlayMode::detectCollisions -- with
// no window and no audio. Rendering is excluded: GameObject::draw needs a live
// RenderWindow, so the numbers here are simulation cost only.
//
// Run from the build directory, so the relative asset paths resolve:
//     cmake --build build --target SpaceInvadersBench
//     cd build && ./SpaceInvadersBench

#include "PhysicsEnginePlayMode.h"
#include "PlayModeObjectLoader.h"
#include "GameObjectSharer.h"
#include "GameObject.h"
#include "InvaderUpdateComponent.h"
#include "BulletUpdateComponent.h"
#include "TransformComponent.h"
#include "NullSoundPlayer.h"
#include "BulletSpawner.h"
#include "WorldState.h"
#include "BitmapStore.h"
#include "FontStore.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace
{
    class BenchObjectSharer : public GameObjectSharer
    {
        public:
            explicit BenchObjectSharer(vector<GameObject>& objects) : m_Objects(objects) {}

            vector<GameObject>& getGameObjectsWithGOS() override { return m_Objects; }

            GameObject& findFirstObjectWithTag(string tag) override
            {
                for (auto& object : m_Objects)
                {
                    if (object.getTagName() == tag) { return object; }
                }
                throw runtime_error("no object tagged " + tag);
            }

        private:
            vector<GameObject>& m_Objects;
    };

    class CountingBulletSpawner : public BulletSpawner
    {
        public:
            long requests = 0;
            void spawnBullet(sf::Vector2f, bool) override { ++requests; }
    };

    struct Stats
    {
        double mean, median, p99, total;
    };

    Stats summarise(vector<double>& samplesMicros)
    {
        sort(samplesMicros.begin(), samplesMicros.end());
        const double total = accumulate(samplesMicros.begin(), samplesMicros.end(), 0.0);
        Stats s;
        s.total = total;
        s.mean = total / static_cast<double>(samplesMicros.size());
        s.median = samplesMicros[samplesMicros.size() / 2];
        s.p99 = samplesMicros[static_cast<size_t>(samplesMicros.size() * 0.99)];
        return s;
    }

    void report(const char* label, Stats s, int frames)
    {
        // 60fps gives a 16667 microsecond budget per frame.
        const double budgetPercent = (s.mean / 16667.0) * 100.0;
        printf("  %-26s mean %8.2f us   median %8.2f us   p99 %8.2f us   %6.3f%% of a 60fps frame\n",
               label, s.mean, s.median, s.p99, budgetPercent);
        (void)frames;
    }
}

// Runs the simulation with the level loaded `copies` times over, and returns the
// mean microseconds spent in detectCollisions per frame.
//
// Collision detection is O(invaders x objects): for every active invader it
// walks the whole bullet range. At the real level size that is nothing, but the
// shape of the growth is the thing worth knowing, because it is what decides
// whether the current implementation is "fine" or "fine for now".
static double measureCollisionScaling(int copies, int frames)
{
    vector<GameObject> objects;
    PlayModeObjectLoader loader;
    for (int i = 0; i < copies; ++i)
    {
        loader.loadGameObjectsForPlayMode("world/level1", objects);
    }

    NullSoundPlayer silence;
    CountingBulletSpawner spawner;
    BenchObjectSharer sharer(objects);

    vector<int> bulletPositions;
    int invaders = 0;
    for (size_t i = 0; i < objects.size(); ++i)
    {
        objects[i].start(&sharer);
        objects[i].setActive();
        if (objects[i].getTag() == ObjectTag::Bullet)
        {
            bulletPositions.push_back(static_cast<int>(i));
        }
        if (objects[i].getTag() == ObjectTag::Invader)
        {
            std::static_pointer_cast<InvaderUpdateComponent>(objects[i].getFirstUpdateComponent())
                ->initializeBulletSpawner(&spawner, static_cast<int>(i), silence);
            ++invaders;
        }
    }

    WorldState::NUM_INVADERS = invaders;
    WorldState::NUM_INVADERS_AT_START = invaders;
    WorldState::LIVES = 3;

    PhysicsEnginePlayMode physics;
    physics.initialize(sharer, silence);

    using clock_type = chrono::steady_clock;
    vector<double> samples;
    samples.reserve(static_cast<size_t>(frames));

    for (int i = 0; i < frames; ++i)
    {
        if (i % 200 == 0)
        {
            WorldState::NUM_INVADERS = invaders;
            WorldState::LIVES = 3;
            for (auto& object : objects) { object.setActive(); }
        }
        const auto start = clock_type::now();
        physics.detectCollisions(objects, bulletPositions);
        const auto end = clock_type::now();
        samples.push_back(chrono::duration<double, micro>(end - start).count());
    }

    printf("  %5zu objects (%4d invaders)  ->  %9.2f us per frame   %7.3f%% of a 60fps frame\n",
           objects.size(), invaders,
           accumulate(samples.begin(), samples.end(), 0.0) / samples.size(),
           (accumulate(samples.begin(), samples.end(), 0.0) / samples.size()) / 16667.0 * 100.0);

    return accumulate(samples.begin(), samples.end(), 0.0) / samples.size();
}

int main()
{
    const int WARMUP = 200;
    const int FRAMES = 20000;
    const float DT = 1.0f / 60.0f;

    vector<GameObject> objects;
    try
    {
        PlayModeObjectLoader loader;
        loader.loadGameObjectsForPlayMode("world/level1", objects);
    }
    catch (const exception& e)
    {
        printf("failed to load level: %s\n", e.what());
        printf("(run this from the build directory so world/ and graphics/ resolve)\n");
        return 1;
    }

    NullSoundPlayer silence;
    CountingBulletSpawner spawner;
    BenchObjectSharer sharer(objects);

    vector<int> bulletPositions;
    int invaders = 0;
    for (size_t i = 0; i < objects.size(); ++i)
    {
        objects[i].start(&sharer);
        objects[i].setActive();
        if (objects[i].getTag() == ObjectTag::Bullet)
        {
            bulletPositions.push_back(static_cast<int>(i));
        }
        if (objects[i].getTag() == ObjectTag::Invader)
        {
            std::static_pointer_cast<InvaderUpdateComponent>(objects[i].getFirstUpdateComponent())
                ->initializeBulletSpawner(&spawner, static_cast<int>(i), silence);
            ++invaders;
        }
    }

    WorldState::WORLD_HEIGHT = 56;
    WorldState::WAVE_NUMBER = 1;
    WorldState::LIVES = 3;
    WorldState::SCORE = 0;
    WorldState::NUM_INVADERS = invaders;
    WorldState::NUM_INVADERS_AT_START = invaders;

    PhysicsEnginePlayMode physics;
    physics.initialize(sharer, silence);

    printf("\nSpaceInvaders simulation benchmark\n");
    printf("  objects: %zu   invaders: %d   bullets: %zu\n",
           objects.size(), invaders, bulletPositions.size());
    printf("  frames: %d (after %d warm-up)   rendering excluded\n\n", FRAMES, WARMUP);

    // Keep every invader alive for the whole run, so the measured work does not
    // quietly shrink as the formation is cleared.
    auto resetCounts = [&]() {
        WorldState::NUM_INVADERS = invaders;
        WorldState::LIVES = 3;
        for (auto& object : objects) { object.setActive(); }
    };

    using clock_type = chrono::steady_clock;

    for (int i = 0; i < WARMUP; ++i)
    {
        for (auto& object : objects) { object.update(DT); }
        physics.detectCollisions(objects, bulletPositions);
    }
    resetCounts();

    vector<double> updateSamples, collisionSamples, frameSamples;
    updateSamples.reserve(FRAMES);
    collisionSamples.reserve(FRAMES);
    frameSamples.reserve(FRAMES);

    for (int i = 0; i < FRAMES; ++i)
    {
        if (i % 500 == 0) { resetCounts(); }

        const auto frameStart = clock_type::now();

        for (auto& object : objects) { object.update(DT); }
        const auto afterUpdate = clock_type::now();

        physics.detectCollisions(objects, bulletPositions);
        const auto frameEnd = clock_type::now();

        updateSamples.push_back(chrono::duration<double, micro>(afterUpdate - frameStart).count());
        collisionSamples.push_back(chrono::duration<double, micro>(frameEnd - afterUpdate).count());
        frameSamples.push_back(chrono::duration<double, micro>(frameEnd - frameStart).count());
    }

    const Stats updateStats = summarise(updateSamples);
    const Stats collisionStats = summarise(collisionSamples);
    const Stats frameStats = summarise(frameSamples);

    report("GameObject::update x all", updateStats, FRAMES);
    report("detectCollisions", collisionStats, FRAMES);
    report("simulation frame (both)", frameStats, FRAMES);

    printf("\n  total simulated: %.1f ms of work across %d frames\n",
           frameStats.total / 1000.0, FRAMES);
    printf("  invader shot requests: %ld\n", spawner.requests);

    printf("\nHow collision detection scales (level replicated N times):\n");
    for (int copies : {1, 2, 4, 8, 16})
    {
        measureCollisionScaling(copies, 2000);
    }
    printf("\n");

    BitmapStore::clear();
    FontStore::clear();
    return 0;
}
