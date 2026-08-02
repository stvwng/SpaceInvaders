#include "Button.h"
#include "FontStore.h"
#include "TextFit.h"

using namespace std;
using namespace sf;

Button::Button(
    Vector2f position,
    float width,
    float height,
    int red,
    int green,
    int blue,
    std::string text
    )
{
    m_Button.setPosition(position);
    m_Button.setFillColor(sf::Color(red, green, blue));
    m_Button.setSize(Vector2f(width, height));

    m_Text = "" + text;

    float textPaddingX = width / 10;
    float textPaddingY = height / 10;
    m_ButtonText.setString(text);
    m_ButtonText.setFont(FontStore::get("fonts/Roboto-Bold.ttf"));

    // height * .7f scales with the button, which reads as safe and is not: it
    // never asks how wide the label is. A button is res.x/20 across whatever the
    // label says, so "Home" spilled past its own red rectangle on a small
    // window while "Play" beside it fitted. Height sets the ceiling; width
    // decides. The minimum is 8 rather than the usual 12 because these boxes
    // are a twentieth of the screen and 12px can already be too tall for them.
    TextFit::fitToWidth(
        m_ButtonText,
        width - (textPaddingX * 2),
        static_cast<unsigned int>(height * .7f),
        8
    );

    m_ButtonText.setPosition(Vector2f((position.x + textPaddingX), (position.y + textPaddingY)));

    m_Collider = FloatRect(position, Vector2f(width, height));
}

void Button::draw(RenderWindow& window)
{
    window.draw(m_Button);
    window.draw(m_ButtonText);
}