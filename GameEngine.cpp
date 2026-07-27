#include "GameEngine.h"
#include "BitmapStore.h"
#include "FontStore.h"

using namespace std;
using namespace sf;

GameEngine::GameEngine()
{
    const VideoMode desktop = VideoMode::getDesktopMode();

#ifdef SPACEINVADERS_FULLSCREEN
    m_Resolution.x = static_cast<float>(desktop.width);
    m_Resolution.y = static_cast<float>(desktop.height);
    const Uint32 style = Style::Fullscreen;
#else
    // Windowed by default. A crash while fullscreen on macOS can leave you
    // with no visible way back to the desktop; -DSPACEINVADERS_FULLSCREEN=ON
    // opts back in.
    m_Resolution.x = static_cast<float>(desktop.width) * 0.8f;
    m_Resolution.y = static_cast<float>(desktop.height) * 0.8f;
    const Uint32 style = Style::Default;
#endif

    m_Window.create(
        VideoMode(static_cast<unsigned int>(m_Resolution.x), static_cast<unsigned int>(m_Resolution.y)),
        "Space Invaders",
        style
    );

    // Without a cap the loop runs as fast as the machine allows. On a 120Hz
    // display that is twice the delta-time resolution the original was tuned
    // against, and on an idle menu it spins a core for nothing.
    m_Window.setFramerateLimit(60);

    m_ScreenManager = make_unique<ScreenManager>(
        Vector2i(static_cast<int>(m_Resolution.x), static_cast<int>(m_Resolution.y)),
        m_SoundEngine
    );
}

GameEngine::~GameEngine()
{
    // Runs before any member is destroyed, so m_Window -- and the OpenGL
    // context it owns -- is still alive here. The texture and font caches are
    // function-local statics that would otherwise be torn down after main()
    // returns, with no context left to release their GPU handles against.
    BitmapStore::clear();
    FontStore::clear();
}

void GameEngine::run()
{
    // The largest step the simulation will take in one frame. A stall -- the
    // first frame after loading a level, or dragging the window -- otherwise
    // produces a single huge dt that teleports every bullet straight through
    // the invaders it should have hit.
    const float maxDeltaTimeSeconds = 0.1f;

    while (m_Window.isOpen())
    {
        m_DT = m_Clock.restart();

        // This variable was named m_FPS but has always held seconds elapsed
        // since the previous frame -- delta time, the reciprocal of a rate.
        // The misnomer propagated into every update(float fps) signature in
        // the codebase.
        m_DeltaTimeSeconds = m_DT.asSeconds();
        if (m_DeltaTimeSeconds > maxDeltaTimeSeconds)
        {
            m_DeltaTimeSeconds = maxDeltaTimeSeconds;
        }

        handleInput();
        update();
        draw();
    }
}

void GameEngine::handleInput()
{
    m_ScreenManager->handleInput(m_Window);
}

void GameEngine::update()
{
    m_ScreenManager->update(m_DeltaTimeSeconds);
}

void GameEngine::draw()
{
    m_Window.clear(Color::Black);
    m_ScreenManager->draw(m_Window);
    m_Window.display();
}