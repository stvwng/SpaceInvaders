#pragma once
#include "InputHandler.h"
#include "PlayerUpdateComponent.h"
#include "TransformComponent.h"

class GameScreen;

class GameInputHandler : public InputHandler
{
    private:
        std::shared_ptr<PlayerUpdateComponent> m_PUC;
        std::shared_ptr<TransformComponent> m_PTC;

        // Previous state of the gamepad fire button, so handleGamepad can fire
        // on the press rather than on every frame it is held.
        bool m_FireButtonWasDown = false;
        
    public:
        void initialize();
        void handleGamepad() override;
        void handleKeyPressed(sf::Event& event, sf::RenderWindow& window) override;
        void handleKeyReleased(sf::Event& event, sf::RenderWindow& window) override;
};