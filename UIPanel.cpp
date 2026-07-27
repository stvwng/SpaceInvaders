#include "UIPanel.h"
#include <memory>

UIPanel::UIPanel(
    Vector2i res,
    int x,
    int y,
    float width,
    float height,
    int alpha,
    int red,
    int green,
    int blue
)
{
    m_UIPanel.setFillColor(sf::Color(red, green, blue, alpha));

    // How big in pixels is the UI panel?
    m_UIPanel.setSize(Vector2f(width, height));

    // How big in pixels is the view?
    m_View.setSize(Vector2f(width, height));

    // Where in pixels does the center of the view focus?
    m_View.setCenter(width / 2, height / 2);

    // Where in the window is the view positioned, as a fraction of 1?
    //
    // This was written as `1.f / (res.x / x)`. Both res.x and x are ints, so
    // the inner division truncated before the float divide ever happened: with
    // res.x = 3024 and x = 1 the intended 0.00033 came out of an integer
    // 3024 by luck, and other combinations collapsed to a division by zero.
    // The fraction wanted here is simply x / res.x.
    float viewportStartX = static_cast<float>(x) / static_cast<float>(res.x);
    float viewportStartY = static_cast<float>(y) / static_cast<float>(res.y);
    float viewportSizeX = width / static_cast<float>(res.x);
    float viewportSizeY = height / static_cast<float>(res.y);

    // Params from left to right
    // StartX and StartY as a fraction of 1
    // SizeX and SizeY as a fraction of 1
    m_View.setViewport(FloatRect(viewportStartX, viewportStartY, viewportSizeX, viewportSizeY));
}

std::vector<std::shared_ptr<Button>> UIPanel::getButtons()
{
    return m_Buttons;
}

void UIPanel::addButton(
    float x,
    float y,
    int width,
    int height,
    int red,
    int green,
    int blue,
    string label
)
{
    m_Buttons.push_back(std::make_shared<Button>(Vector2f(x,y), width, height, red, green, blue, label));
}

void UIPanel::draw(RenderWindow& window)
{
    window.setView(m_View);
    if (!m_Hidden)
    {
        window.draw(m_UIPanel);
        for (auto it = m_Buttons.begin(); it != m_Buttons.end(); ++it)
        {
            (*it)->draw(window);
        }
    }
}

void UIPanel::show()
{
    m_Hidden = false;
}

void UIPanel::hide()
{
    m_Hidden = true;
}