#pragma once
#include "UIPanel.h"

class GameUIPanel : public UIPanel
{
    public:
        GameUIPanel(sf::Vector2i res);
        void draw(sf::RenderWindow& window) override;
};