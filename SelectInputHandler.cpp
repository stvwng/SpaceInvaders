#include "SelectInputHandler.h"
#include "WorldState.h"
#include <iostream>


void SelectInputHandler::handleKeyPressed(Event&, RenderWindow& window)
{
    // Quit game
    if (Keyboard::isKeyPressed(Keyboard::Escape))
    {
        window.close();
    }
}

void SelectInputHandler::handleLeftClick(std::string& buttonInteractedWith, RenderWindow& window)
{
    if (buttonInteractedWith == "Play")
    {
        getPointerToScreenManagerRemoteControl()->shareSoundPlayer().playClick();
        WorldState::WAVE_NUMBER = 0;
        getPointerToScreenManagerRemoteControl()->loadLevelInPlayMode("level1");
    }

    if (buttonInteractedWith == "Quit")
    {
        getPointerToScreenManagerRemoteControl()->shareSoundPlayer().playClick();
        window.close();
    }
}