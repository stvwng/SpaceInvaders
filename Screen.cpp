#include "Screen.h"

void Screen::initialize(){}

void Screen::addPanel(
    unique_ptr<UIPanel> uip,
    ScreenManagerRemoteControl* smrc,
    shared_ptr<InputHandler> ih
)
{
    ih->initializeInputHandler(smrc, uip->getButtons(), &uip->m_View, this);
    // Use MOVE b/c uip is a unique ptr and COPY is not allowed
    m_Panels.push_back(move(uip));
    m_InputHandlers.push_back(ih);
}

void Screen::handleInput(RenderWindow& window)
{
    Event event;
    auto it = m_InputHandlers.begin();
    auto end = m_InputHandlers.end();
    while (window.pollEvent(event))
    {
        for (it; it != end; ++it)
        {
            (*it)->handleInput(window, event);
        }
    }
}

void Screen::update(float fps){}

void Screen::draw(RenderWindow& window)
{
    auto it = m_Panels.begin();
    auto end = m_Panels.end();
    for (it; it != end; ++it)
    {
        (*it)->draw(window);
    }
}