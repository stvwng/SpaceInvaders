#pragma once
#include <SFML/Graphics.hpp>
#include "ScreenManager.h"
#include "SoundEngine.h"

using namespace sf;

class GameEngine
{
    private:
        Clock m_Clock;
        Time m_DT;
        RenderWindow m_Window;

        // Declared before m_ScreenManager on purpose: members initialise in
        // declaration order, and the ScreenManager is handed a reference to this.
        SoundEngine m_SoundEngine;

        unique_ptr<ScreenManager> m_ScreenManager;

        float m_DeltaTimeSeconds = 0;
        Vector2f m_Resolution;

        void handleInput();
        void update();
        void draw();

    public:
        GameEngine();
        ~GameEngine();
        void run();
};