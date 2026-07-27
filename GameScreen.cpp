#include "GameScreen.h"
#include "GameUIPanel.h"
#include "GameInputHandler.h"
#include "GameOverUIPanel.h"
#include "GameObject.h"
#include "WorldState.h"
#include "BulletUpdateComponent.h"
#include "InvaderUpdateComponent.h"

class BulletSpawner;

// Storage for GameScreen's static flag. This lived in GameOverUIPanel.cpp,
// which is the panel that *reads* it -- storage now sits with the class that
// owns the state.
bool GameScreen::m_GameOver = false;

GameScreen::GameScreen(ScreenManagerRemoteControl* smrc, Vector2i res)
{
    m_GIH = make_shared<GameInputHandler>();
    auto guip = make_unique<GameUIPanel>(res);
    addPanel(std::move(guip), smrc, m_GIH);

    auto m_GOIH = make_shared<GameOverInputHandler>();
    auto gouip = make_unique<GameOverUIPanel>(res);
    addPanel(std::move(gouip), smrc, m_GOIH);

    m_ScreenManagerRemoteControl = smrc;

    // VideoMode's width and height are unsigned int, so this used to be integer
    // division: on any display narrower than 2:1 the ratio truncated to 1 and
    // WORLD_HEIGHT became 100 instead of ~56. Every piece of world geometry --
    // where the player is clamped, when a bullet leaves the screen -- was
    // computed from that wrong number.
    const float screenRatio =
        static_cast<float>(VideoMode::getDesktopMode().width) /
        static_cast<float>(VideoMode::getDesktopMode().height);

    WorldState::WORLD_HEIGHT = static_cast<int>(WorldState::WORLD_WIDTH / screenRatio);

    m_View.setSize(WorldState::WORLD_WIDTH, WorldState::WORLD_HEIGHT);

    m_View.setCenter(Vector2f(WorldState::WORLD_WIDTH / 2, WorldState::WORLD_HEIGHT / 2));

    m_BackgroundTexture.loadFromFile("graphics/background.png");
    m_BackgroundSprite.setTexture(m_BackgroundTexture);
    auto textureSize = m_BackgroundSprite.getTexture()->getSize();
    m_BackgroundSprite.setScale(float(m_View.getSize().x) / textureSize.x, float(m_View.getSize().y / textureSize.y));
}

void GameScreen::initialize()
{
    m_GIH->initialize();
    m_PhysicsEnginePlayMode.initialize(m_ScreenManagerRemoteControl->shareGameObjectSharer());

    WorldState::NUM_INVADERS = 0;

    // These describe the GameObject vector, which is rebuilt on every level
    // load. Without clearing, the list grew by another full set of bullet
    // indices on each wave and each restart, and m_NextBullet kept walking
    // into indices belonging to a previous level.
    m_BulletObjectLocations.clear();
    m_NextBullet = 0;

    // Store bullet locations and initialize the invaders' BulletSpawners
    int i = 0;
    auto it = m_ScreenManagerRemoteControl->getGameObjects().begin();
    auto end = m_ScreenManagerRemoteControl->getGameObjects().end();
    for (; it != end; ++it)
    {
        if (it->getTag() == "bullet")
        {
            m_BulletObjectLocations.push_back(i);
        }

        if (it->getTag() == "invader")
        {
            static_pointer_cast<InvaderUpdateComponent>(it->getFirstUpdateComponent())->initializeBulletSpawner(getBulletSpawner(), i);
            WorldState::NUM_INVADERS++;
        }
        ++i;
    }

    m_GameOver = false;

    if (WorldState::WAVE_NUMBER == 0)
    {
        WorldState::NUM_INVADERS_AT_START = WorldState::NUM_INVADERS;
        WorldState::WAVE_NUMBER = 1;
        WorldState::LIVES = 3;
        WorldState::SCORE = 0;
    }
}

void GameScreen::update(float dt)
{
    Screen::update(dt);

    if (!m_GameOver)
    {
        if (m_WaitingToSpawnBulletForPlayer)
        {
            static_pointer_cast<BulletUpdateComponent>(
                m_ScreenManagerRemoteControl->
                getGameObjects()
                [m_BulletObjectLocations[m_NextBullet]].
                getFirstUpdateComponent())->
                spawnForPlayer(m_PlayerBulletSpawnLocation);
            m_WaitingToSpawnBulletForPlayer = false;
            m_NextBullet++;

            if (m_NextBullet == m_BulletObjectLocations.size())
            {
                m_NextBullet = 0;
            }    
        }

        if (m_WaitingToSpawnBulletForInvader)
        {
            static_pointer_cast<BulletUpdateComponent>(
                m_ScreenManagerRemoteControl->
                getGameObjects()
                [m_BulletObjectLocations[m_NextBullet]].
                getFirstUpdateComponent())->
                spawnForInvader(m_InvaderBulletSpawnLocation);

            m_WaitingToSpawnBulletForInvader = false;
            m_NextBullet++;

            if (m_NextBullet == m_BulletObjectLocations.size())
            {
                m_NextBullet = 0;
            }
        }

        auto it = m_ScreenManagerRemoteControl->getGameObjects().begin();
        auto end = m_ScreenManagerRemoteControl->getGameObjects().end();

        for (; it != end; ++it)
        {
            it->update(dt);
        }

        m_PhysicsEnginePlayMode.detectCollisions(
            m_ScreenManagerRemoteControl->getGameObjects(),
            m_BulletObjectLocations
        );

        if (WorldState::NUM_INVADERS <= 0)
        {
            WorldState::WAVE_NUMBER++;
            m_ScreenManagerRemoteControl->loadLevelInPlayMode("level1");
        }

        // <= rather than ==: two hits landing in the same frame would step
        // straight past zero and the game would never end.
        if (WorldState::LIVES <= 0)
        {
            m_GameOver = true;
        }
    }
}

void GameScreen::draw(RenderWindow& window)
{
    // Change to this screen's view to draw
    window.setView(m_View);
    window.draw(m_BackgroundSprite);

    // Draw the GameObject instances
    auto it = m_ScreenManagerRemoteControl->getGameObjects().begin();
    auto end = m_ScreenManagerRemoteControl->getGameObjects().end();
    for (; it != end; ++it)
    {
        it->draw(window);
    }

    // Draw UIPanel view(s)
    Screen::draw(window);
}

BulletSpawner* GameScreen::getBulletSpawner()
{
    return this;
}