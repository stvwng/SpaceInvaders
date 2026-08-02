#include <doctest/doctest.h>
#include "PhysicsEnginePlayMode.h"
#include "PlayModeObjectLoader.h"
#include "GameObjectSharer.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "BulletUpdateComponent.h"
#include "InvaderUpdateComponent.h"
#include "NullSoundPlayer.h"
#include "WorldState.h"
#include "BulletSpawner.h"
#include <string>
#include <vector>
#include <stdexcept>

// Collision logic was untestable until SoundEngine stopped being a raw-static
// singleton: every hit called SoundEngine::playInvaderExplode() through a global
// pointer, so a test either made noise or crashed on a null instance. With a
// SoundPlayer& injected at initialize(), NullSoundPlayer makes the whole engine
// exercisable in silence.

namespace
{
    std::string fixture(const std::string& name)
    {
        return std::string(SPACEINVADERS_TEST_FIXTURES) + "/" + name;
    }

    // Minimal GameObjectSharer over a plain vector, so these tests do not need
    // a LevelManager (which hardcodes the "world/" directory).
    class TestObjectSharer : public GameObjectSharer
    {
        public:
            explicit TestObjectSharer(std::vector<GameObject>& objects) : m_Objects(objects) {}

            std::vector<GameObject>& getGameObjectsWithGOS() override { return m_Objects; }

            GameObject& findFirstObjectWithTag(std::string tag) override
            {
                for (auto& object : m_Objects)
                {
                    if (object.getTagName() == tag) { return object; }
                }
                throw std::runtime_error("no object tagged " + tag);
            }

        private:
            std::vector<GameObject>& m_Objects;
    };

    class NullBulletSpawner : public BulletSpawner
    {
        public:
            void spawnBullet(sf::Vector2f, bool) override {}
    };

    // Everything a physics test needs, wired the way GameScreen wires it.
    struct World
    {
        std::vector<GameObject> objects;
        std::vector<int> bulletPositions;
        NullSoundPlayer silence;
        NullBulletSpawner spawner;
        PhysicsEnginePlayMode physics;

        World()
        {
            PlayModeObjectLoader loader;
            loader.loadGameObjectsForPlayMode(fixture("physics_level"), objects);

            TestObjectSharer sharer(objects);
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
                }
            }

            // A fresh baseline: WorldState is global, so every test states it.
            WorldState::WORLD_HEIGHT = 56;
            WorldState::WAVE_NUMBER = 1;
            WorldState::LIVES = 3;
            WorldState::SCORE = 0;
            WorldState::NUM_INVADERS = 2;
            WorldState::NUM_INVADERS_AT_START = 2;
        }

        // initialize() must run after the sharer used above has gone away, so
        // it takes its own -- mirroring GameScreen, which re-resolves on every
        // level load.
        void begin(TestObjectSharer& sharer) { physics.initialize(sharer, silence); }

        void step() { physics.detectCollisions(objects, bulletPositions); }

        GameObject& first(const std::string& tag)
        {
            for (auto& object : objects)
            {
                if (object.getTagName() == tag) { return object; }
            }
            throw std::runtime_error("no object tagged " + tag);
        }

        BulletUpdateComponent& bullet(size_t which)
        {
            return *std::static_pointer_cast<BulletUpdateComponent>(
                objects[bulletPositions[which]].getFirstUpdateComponent());
        }

        GameObject& bulletObject(size_t which) { return objects[bulletPositions[which]]; }
    };
}

TEST_CASE("a live player bullet on an invader kills it exactly once")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& invader = w.first("invader");
    const sf::Vector2f invaderPos = invader.getTransformComponent()->getLocation();

    // spawnForPlayer positions the bullet just above the point given and marks
    // it in flight and player-owned.
    w.bullet(0).spawnForPlayer(sf::Vector2f(invaderPos.x, invaderPos.y + 2.f));

    w.step();

    CHECK(WorldState::NUM_INVADERS == 1);
    CHECK(WorldState::SCORE == 1);
    CHECK_FALSE(invader.isActive());

    // The invader is inactive now, so later frames must not score again.
    w.step();
    w.step();
    CHECK(WorldState::NUM_INVADERS == 1);
    CHECK(WorldState::SCORE == 1);
}

TEST_CASE("two bullets overlapping one invader still only kill it once")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& invader = w.first("invader");
    const sf::Vector2f invaderPos = invader.getTransformComponent()->getLocation();

    // Both bullets in flight and overlapping the same invader in the same
    // frame. Without the `break` in detectInvaderCollisions the inner loop
    // carries on past the first hit and scores the same invader twice, driving
    // NUM_INVADERS below the true count and advancing the wave early.
    //
    // One bullet is not enough to exercise this -- which is what the original
    // version of this test got wrong.
    w.bullet(0).spawnForPlayer(sf::Vector2f(invaderPos.x, invaderPos.y + 2.f));
    w.bullet(1).spawnForPlayer(sf::Vector2f(invaderPos.x, invaderPos.y + 2.f));

    REQUIRE(w.bulletObject(0).getEncompassingRectCollider()
                .intersects(invader.getEncompassingRectCollider()));
    REQUIRE(w.bulletObject(1).getEncompassingRectCollider()
                .intersects(invader.getEncompassingRectCollider()));

    w.step();

    CHECK(WorldState::NUM_INVADERS == 1);
    CHECK(WorldState::SCORE == 1);
}

TEST_CASE("a bullet that is not in flight cannot kill an invader")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& invader = w.first("invader");
    const sf::Vector2f invaderPos = invader.getTransformComponent()->getLocation();

    // The bullet must be player-*owned* but no longer in flight, otherwise the
    // ownership check alone rejects it and m_IsSpawned is never the deciding
    // factor. Spawning then de-spawning is exactly the state a bullet is left
    // in after it flies off the top of the screen.
    w.bullet(0).spawnForPlayer(sf::Vector2f(invaderPos.x, invaderPos.y + 2.f));
    w.bullet(0).deSpawn();

    REQUIRE(w.bullet(0).m_BelongsToPlayer);
    REQUIRE_FALSE(w.bullet(0).m_IsSpawned);
    REQUIRE(w.bulletObject(0).getEncompassingRectCollider()
                .intersects(invader.getEncompassingRectCollider()));

    w.step();

    CHECK(WorldState::NUM_INVADERS == 2);
    CHECK(WorldState::SCORE == 0);
    CHECK(invader.isActive());
}

TEST_CASE("an invader's own bullet cannot kill an invader")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& invader = w.first("invader");
    const sf::Vector2f invaderPos = invader.getTransformComponent()->getLocation();

    // Invader fire passes straight through the formation. This needs its own
    // case: the "not in flight" test uses a de-spawned player bullet, so it
    // exercises m_IsSpawned but never the ownership half of the condition.
    w.bullet(0).spawnForInvader(sf::Vector2f(invaderPos.x, invaderPos.y - 2.f));

    REQUIRE(w.bullet(0).m_IsSpawned);
    REQUIRE_FALSE(w.bullet(0).m_BelongsToPlayer);
    REQUIRE(w.bulletObject(0).getEncompassingRectCollider()
                .intersects(invader.getEncompassingRectCollider()));

    w.step();

    CHECK(WorldState::NUM_INVADERS == 2);
    CHECK(WorldState::SCORE == 0);
    CHECK(invader.isActive());
}

TEST_CASE("the player's own bullet does not cost the player a life")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& player = w.first("Player");
    const sf::Vector2f playerPos = player.getTransformComponent()->getLocation();
    const float bulletHeight = w.bulletObject(0).getTransformComponent()->getSize().y;

    // spawnForPlayer places the bullet one bullet-height *above* the point
    // given, and moves the collider with it. Spawning from just below the
    // player therefore lands it squarely on the player -- which is what really
    // happens when the player fires.
    w.bullet(0).spawnForPlayer(sf::Vector2f(playerPos.x, playerPos.y + bulletHeight));

    REQUIRE(w.bulletObject(0).getEncompassingRectCollider()
                .intersects(player.getEncompassingRectCollider()));

    w.step();

    CHECK(WorldState::LIVES == 3);
}

TEST_CASE("an invader bullet on the player costs exactly one life")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& player = w.first("Player");
    const sf::Vector2f playerPos = player.getTransformComponent()->getLocation();
    const float bulletHeight = w.bulletObject(0).getTransformComponent()->getSize().y;

    // spawnForInvader places the bullet one bullet-height *below* the point
    // given, so spawn from above the player to land on it.
    w.bullet(0).spawnForInvader(sf::Vector2f(playerPos.x, playerPos.y - bulletHeight));

    REQUIRE(w.bulletObject(0).getEncompassingRectCollider()
                .intersects(player.getEncompassingRectCollider()));

    w.step();
    CHECK(WorldState::LIVES == 2);

    // deSpawn() on the hit means the same bullet cannot keep draining lives on
    // subsequent frames.
    w.step();
    CHECK(WorldState::LIVES == 2);
}

TEST_CASE("an invader reaching the player is removed from the invader count")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& player = w.first("Player");
    GameObject& invader = w.first("invader");

    invader.getTransformComponent()->getLocation() = player.getTransformComponent()->getLocation();
    std::static_pointer_cast<RectColliderComponent>(
        invader.getComponentByTypeAndSpecificType(ComponentType::Collider, ComponentSpecificType::Rect))
        ->setOrMoveCollider(
            player.getTransformComponent()->getLocation().x,
            player.getTransformComponent()->getLocation().y,
            2.f, 2.f);

    w.step();

    CHECK(WorldState::LIVES == 2);
    CHECK_FALSE(invader.isActive());

    // This is the part that was missing: the invader was deactivated but never
    // subtracted, so a wave ended that way could never be cleared.
    CHECK(WorldState::NUM_INVADERS == 1);
}

TEST_CASE("hitting the right wall makes the formation drop and reverse")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& invader = w.first("invader");
    auto invaderUpdate = std::static_pointer_cast<InvaderUpdateComponent>(invader.getFirstUpdateComponent());
    auto transform = invader.getTransformComponent();

    REQUIRE(invaderUpdate->isMovingRight());
    const float startY = transform->getLocation().y;

    // Push it past the right edge of the world while still travelling right.
    transform->getLocation().x = static_cast<float>(WorldState::WORLD_WIDTH) - transform->getSize().x;

    // Frame 1 records the wall hit; nothing moves yet.
    w.step();
    CHECK(invaderUpdate->isMovingRight());
    CHECK(transform->getLocation().y == doctest::Approx(startY));

    // Frame 2 performs the drop and the reversal -- the two-frame handshake in
    // handleInvaderDirection.
    w.step();
    CHECK_FALSE(invaderUpdate->isMovingRight());
    CHECK(transform->getLocation().y > startY);
}

// The invaders win by arriving. Nothing used to bound an invader's y at all:
// dropDownAndReverse just kept adding a row-height, so the formation walked
// through the player's row, below it, and off the bottom of the view. The player
// is clamped to WORLD_HEIGHT / 2, so those invaders could never be shot or
// collided with again -- NUM_INVADERS could not reach zero and LIVES stopped
// falling, and the game ran forever with nothing left to play against.

TEST_CASE("an invader reaching the player's row ends the game")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& player = w.first("Player");
    GameObject& invader = w.first("invader");
    auto transform = invader.getTransformComponent();

    const float playerTop = player.getTransformComponent()->getLocation().y;

    // Clear of the player in x, so this measures the arrival and not the
    // ram-the-player collision, which is a different code path with a different
    // outcome (one life, one dead invader).
    transform->getLocation().x = 10.f;

    // Bottom edge exactly level with the player's top edge: the boundary case,
    // which the >= is there to catch.
    transform->getLocation().y = playerTop - transform->getSize().y;

    REQUIRE_FALSE(w.physics.invadersReachedPlayer());
    REQUIRE_FALSE(invader.getEncompassingRectCollider()
                      .intersects(player.getEncompassingRectCollider()));

    w.step();

    CHECK(w.physics.invadersReachedPlayer());
}

TEST_CASE("invaders above the player's row do not end the game")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& player = w.first("Player");
    GameObject& invader = w.first("invader");
    auto transform = invader.getTransformComponent();

    // Where the level actually starts them.
    w.step();
    CHECK_FALSE(w.physics.invadersReachedPlayer());

    // Half a world unit short of the row: an off-by-one here would end the game
    // a frame early, or on the very first frame if the comparison were reversed.
    transform->getLocation().y =
        player.getTransformComponent()->getLocation().y - transform->getSize().y - 0.5f;

    w.step();
    CHECK_FALSE(w.physics.invadersReachedPlayer());
}

TEST_CASE("a new wave clears the invasion flag")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& player = w.first("Player");
    auto transform = w.first("invader").getTransformComponent();

    transform->getLocation().x = 10.f;
    transform->getLocation().y =
        player.getTransformComponent()->getLocation().y - transform->getSize().y;

    w.step();
    REQUIRE(w.physics.invadersReachedPlayer());

    // A level load re-initializes the engine. Without the reset the next wave
    // -- and every restart after it -- would begin already lost.
    w.begin(sharer);
    CHECK_FALSE(w.physics.invadersReachedPlayer());
}

TEST_CASE("invaders left to descend end the game before they pass the player")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    GameObject& player = w.first("Player");
    const float playerTop = player.getTransformComponent()->getLocation().y;
    const float playerBottom = playerTop + player.getTransformComponent()->getSize().y;

    // The reported bug, driven through the real descent path rather than by
    // placing an invader on the line by hand: nobody shoots, the formation
    // bounces off the walls, drops a row each time, and must run out of screen.
    const float dt = 1.f / 60.f;
    const int frameLimit = 20000;
    int frames = 0;

    while (!w.physics.invadersReachedPlayer() && frames < frameLimit)
    {
        for (auto& object : w.objects) { object.update(dt); }
        w.step();
        ++frames;
    }

    // Before the fix this loop ran to the limit: nothing bounded an invader's y,
    // so the game had no way to end once the formation was past the player.
    CHECK(w.physics.invadersReachedPlayer());
    INFO("frames simulated: ", frames);
    CHECK(frames < frameLimit);

    // And it must end on arrival, not after. No invader may be below the player
    // on the frame the game is declared over -- one that is can never be shot
    // (the player is clamped to WORLD_HEIGHT / 2) nor collided with, which is
    // what made the wave unfinishable.
    for (auto& object : w.objects)
    {
        if (object.getTag() != ObjectTag::Invader || !object.isActive()) { continue; }
        CHECK(object.getTransformComponent()->getLocation().y <= playerBottom);
    }
}

TEST_CASE("collision detection copes with a level that has no bullets")
{
    World w;
    TestObjectSharer sharer(w.objects);
    w.begin(sharer);

    // advance(it, bulletPositions[0]) used to run unconditionally, reading past
    // the end of an empty vector.
    w.bulletPositions.clear();
    CHECK_NOTHROW(w.step());
}
