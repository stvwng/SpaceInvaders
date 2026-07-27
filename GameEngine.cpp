#include "GameEngine.h"

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

    m_ScreenManager = unique_ptr<ScreenManager>(new ScreenManager(
        Vector2i(static_cast<int>(m_Resolution.x), static_cast<int>(m_Resolution.y))
    ));
}

void GameEngine::run()
{
    while (m_Window.isOpen())
    {
        m_DT = m_Clock.restart();
        m_FPS = m_DT.asSeconds();
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
    m_ScreenManager->update(m_FPS);
}

void GameEngine::draw()
{
    m_Window.clear(Color::Black);
    m_ScreenManager->draw(m_Window);
    m_Window.display();
}