#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "InputHandler.h"
#include "UIPanel.h"
#include "ScreenManagerRemoteControl.h"

class InputHandler;

class Screen
{
    private:
        vector<shared_ptr<InputHandler>> m_InputHandlers;
        vector<unique_ptr<UIPanel>> m_Panels;

    protected:
        void addPanel(
            unique_ptr<UIPanel> uip,
            ScreenManagerRemoteControl* smrc,
            shared_ptr<InputHandler> ih
        );

    public:
        // Screens are owned as unique_ptr<Screen> in ScreenManager and deleted
        // through that base pointer. Without this, that is undefined behaviour.
        virtual ~Screen() = default;

        virtual void initialize();
        void virtual update(float dt);
        void virtual draw(RenderWindow& window);
        void handleInput(RenderWindow& window);

        View m_View;
};