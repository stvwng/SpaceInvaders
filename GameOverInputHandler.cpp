#include "GameOverInputHandler.h"
#include "WorldState.h"
#include <iostream>

using namespace std;
using namespace sf;

void GameOverInputHandler::handleKeyPressed(Event& event, RenderWindow&)
{
    if (event.key.code == Keyboard::Escape)
    {
        getPointerToScreenManagerRemoteControl()->shareSoundPlayer().playClick();
        getPointerToScreenManagerRemoteControl()->switchScreens("Select");
    }
}

void GameOverInputHandler::handleLeftClick(std::string& buttonInteractedWith, RenderWindow&)
{
    if (buttonInteractedWith == "Play")
    {
        getPointerToScreenManagerRemoteControl()->shareSoundPlayer().playClick();
        WorldState::WAVE_NUMBER = 0;
        getPointerToScreenManagerRemoteControl()->loadLevelInPlayMode("level1");
    }
    else if (buttonInteractedWith == "Home")
    {
        getPointerToScreenManagerRemoteControl()->shareSoundPlayer().playClick();
        getPointerToScreenManagerRemoteControl()->switchScreens("Select");
    }
}