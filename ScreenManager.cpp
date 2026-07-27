#include "ScreenManager.h"

using namespace std;
using namespace sf;

ScreenManager::ScreenManager(Vector2i res, SoundPlayer& soundPlayer)
    : m_SoundPlayer(soundPlayer)
{
    m_Screens["Game"] = make_unique<GameScreen>(this, res);
    m_Screens["Select"] = make_unique<SelectScreen>(this, res);
}

void ScreenManager::handleInput(RenderWindow& window)
{
    m_Screens[m_CurrentScreen]->handleInput(window);
}

void ScreenManager::update(float dt)
{
    m_Screens[m_CurrentScreen]->update(dt);
}

void ScreenManager::draw(RenderWindow& window)
{
    m_Screens[m_CurrentScreen]->draw(window);
}