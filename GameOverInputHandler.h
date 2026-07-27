#pragma once
#include "InputHandler.h"

class GameOverInputHandler : public InputHandler
{
    public:
        void handleKeyPressed(sf::Event& event, sf::RenderWindow& window) override;
        void handleLeftClick(std::string& buttonInteractedWith, sf::RenderWindow& window) override;
};