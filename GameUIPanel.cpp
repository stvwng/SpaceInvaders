#include "GameUIPanel.h"
#include "FontStore.h"
#include "TextFit.h"
#include <sstream>
#include "WorldState.h"

using namespace std;
using namespace sf;


GameUIPanel::GameUIPanel(Vector2i res) :
    UIPanel(
        res,
        1, // the left
        1, // the top
        res.x / 3, // 1/3 width screen
        res.y / 12,
        50, 255, 255, 255 // a, r, g, b
    ) // initialize with parent class constructor
{
    // This panel has no buttons, so the inherited m_ButtonPadding would be a
    // misnomer -- but the 1% of screen width it uses elsewhere is the right
    // inset, and it scales where the previous hardcoded 15px did not.
    const float padding = res.x / 100.f;

    m_Text.setFillColor(sf::Color(0, 255, 0, 255));
    m_Text.setFont(FontStore::get("fonts/Roboto-Bold.ttf"));

    // draw() rewrites this string every frame, so the size is settled once here
    // against the widest reading the HUD can ever show. Re-fitting per frame
    // would both cost a measurement on every draw and visibly resize the text
    // under the player the moment the score gained a digit.
    m_Text.setString("Score: 999999   Lives: 9   Wave: 99");
    TextFit::fitToWidth(
        m_Text,
        m_Width - (padding * 2),
        TextFit::sizeForHeight(60, m_Height - padding)
    );

    // Left-aligned, unlike the title panels: a centred HUD would slide sideways
    // as the score grows.
    m_Text.setPosition(Vector2f(padding, padding));
    m_Text.setString("Score: 0   Lives: 3   Wave: 1");
}

void GameUIPanel::draw(RenderWindow& window)
{
    UIPanel::draw(window);

    std::stringstream ss;
    ss << "Score: " << WorldState::SCORE << "   Lives: "
       << WorldState::LIVES << "   Wave: "
       << WorldState::WAVE_NUMBER;
    m_Text.setString(ss.str());

    window.draw(m_Text);
}