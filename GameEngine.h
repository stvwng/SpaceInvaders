#pragma once
#include <SFML/Graphics.hpp>
#include "ScreenManager.h"
#include "SoundEngine.h"


class GameEngine
{
    private:
        sf::Clock m_Clock;
        sf::Time m_DT;
        sf::RenderWindow m_Window;

        // Declared before m_ScreenManager on purpose: members initialise in
        // declaration order, and the ScreenManager is handed a reference to this.
        SoundEngine m_SoundEngine;

        std::unique_ptr<ScreenManager> m_ScreenManager;

        float m_DeltaTimeSeconds = 0;
        sf::Vector2f m_Resolution;

        void handleInput();
        void update();
        void draw();

    public:
        GameEngine();
        ~GameEngine();
        void run();
};