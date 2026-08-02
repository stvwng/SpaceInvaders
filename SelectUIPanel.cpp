#include "SelectUIPanel.h"
#include "FontStore.h"
#include "TextFit.h"
#include <iostream>

using namespace std;
using namespace sf;

SelectUIPanel::SelectUIPanel(Vector2i res) :
    // Create new UIPanel by calling super-class constructor
    UIPanel(
        res,
        (res.x / 10) * 2, // start 2/10 across
        res.y / 3, // 1/3 of the resolution from the top
        (res.x / 10) * 6, // as wide as 60% of the resolution
        res.y / 3, // and as tall as 1/3 of the resolution
        50, 255, 255, 255 // a, r, g, b
    )
{
    m_ButtonWidth = res.x / 20;
    m_ButtonHeight = res.y / 20;
    m_ButtonPadding = res.x / 100;

    m_Text.setFillColor(sf::Color(0, 255, 0, 255));
    m_Text.setString("SPACE INVADERS");

    m_Text.setFont(FontStore::get("fonts/Roboto-Bold.ttf"));

    // 160px was the design size and is still the ceiling, but the panel is 60%
    // of a resolution we do not control: "SPACE INVADERS" in Roboto-Bold needs
    // about 1400px at that size, so it only ever fitted on a display wider than
    // ~2400px and was clipped by the panel's viewport on every smaller one.
    const float titleTop = m_ButtonHeight + (m_ButtonPadding * 2);
    TextFit::fitToWidth(
        m_Text,
        m_Width - (m_ButtonPadding * 2),
        TextFit::sizeForHeight(160, m_Height - titleTop)
    );
    m_Text.setPosition(Vector2f(centredTextX(m_Text), titleTop));

    initializeButtons();
}

void SelectUIPanel::initializeButtons()
{
    addButton(
        m_ButtonPadding,
        m_ButtonPadding,
        m_ButtonWidth,
        m_ButtonHeight,
        0, 255, 0,
        "Play"
    );

    addButton(
        m_ButtonWidth + (m_ButtonPadding * 2),
        m_ButtonPadding,
        m_ButtonWidth,
        m_ButtonHeight,
        255, 0, 0,
        "Quit"
    );
}

void SelectUIPanel::draw(RenderWindow& window)
{
    show();
    UIPanel::draw(window);
    window.draw(m_Text);
}