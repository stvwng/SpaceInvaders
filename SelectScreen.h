#pragma once
#include "Screen.h"

class SelectScreen : public Screen
{
    private:
        ScreenManagerRemoteControl* m_ScreenManagerRemoteControl;
        sf::Texture m_BackgroundTexture;
        sf::Sprite m_BackgroundSprite;

    public:
        SelectScreen(ScreenManagerRemoteControl* smrc, sf::Vector2i res);
        void draw(sf::RenderWindow& window) override;
};