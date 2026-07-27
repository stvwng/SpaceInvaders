#include <sstream>
#include "InputHandler.h"

using namespace sf;
using namespace std;

void InputHandler::initializeInputHandler(
    ScreenManagerRemoteControl* sw,
    vector<shared_ptr<Button>> buttons,
    View* pointerToUIView,
    Screen* parentScreen
)
{
    m_ScreenManagerRemoteControl = sw;
    m_Buttons = buttons;
    m_PointerToUIPanelView = pointerToUIView;
    m_ParentScreen = parentScreen;
}

void InputHandler::handleInput(RenderWindow& window, Event& event)
{
    // Handle key presses
    if (event.type == Event::KeyPressed)
    {
        handleKeyPressed(event, window);
    }

    if (event.type == Event::KeyReleased)
    {
        handleKeyReleased(event, window);
    }

    // Handle left mouse click released
    if (event.type == Event::MouseButtonReleased)
    {
        auto end = m_Buttons.end();

        for (auto i = m_Buttons.begin(); i != end; ++i)
        {
            // Mouse::getPosition() with no argument returns desktop
            // coordinates; the window overload returns coordinates relative to
            // the window, which is what mapPixelToCoords expects. The two only
            // agree when the window is at the origin -- i.e. fullscreen.
            if ((*i)->m_Collider.contains(window.mapPixelToCoords(Mouse::getPosition(window), (*getPointerToUIView()))))
            {
                // Capture text of the button that was interacted with and pass to the specialized version of this class
                handleLeftClick((*i)->m_Text, window);
                break;
            }
        }
    }

    handleGamepad();
}

void InputHandler::handleGamepad(){} // do nothing unless handled by derived class
void InputHandler::handleKeyPressed(Event&, RenderWindow&){} // do nothing unless handled by derived class
void InputHandler::handleKeyReleased(Event&, RenderWindow&){} // do nothing unless handled by derived class
void InputHandler::handleLeftClick(string&, RenderWindow&){} // do nothing unless handled by derived class

View* InputHandler::getPointerToUIView()
{
    return m_PointerToUIPanelView;
}

ScreenManagerRemoteControl* InputHandler::getPointerToScreenManagerRemoteControl()
{
    return m_ScreenManagerRemoteControl;
}

Screen* InputHandler::getParentScreen()
{
    return m_ParentScreen;
}