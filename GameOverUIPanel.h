#pragma once
#include "UIPanel.h"

class GameOverUIPanel : public UIPanel
{
    private:
        void initializeButtons();

    public:
        GameOverUIPanel(sf::Vector2i res);
        void draw(sf::RenderWindow& window) override;
};