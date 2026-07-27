#include "GameInputHandler.h"
#include "GameScreen.h"

class BulletSpawner;

// m_PUC and m_PTC were never assigned -- this body was empty. InputHandler
// calls handleGamepad() on every event, which dereferenced the null m_PUC, so
// the game crashed on the very first input. Resolve the player's components
// through the GameObjectSharer, the same way PhysicsEnginePlayMode does.
//
// GameScreen::initialize() calls this after every level load, which is also
// what keeps these pointers valid: the GameObject vector is rebuilt on each
// load, so anything cached from the previous level must be re-resolved.
void GameInputHandler::initialize()
{
    GameObjectSharer& gos = getPointerToScreenManagerRemoteControl()->shareGameObjectSharer();
    GameObject& player = gos.findFirstObjectWithTag("Player");

    m_PUC = static_pointer_cast<PlayerUpdateComponent>(
        player.getComponentByTypeAndSpecificType("update", "player")
    );

    m_PTC = static_pointer_cast<TransformComponent>(
        player.getComponentByTypeAndSpecificType("transform", "transform")
    );
}

void GameInputHandler::handleGamepad()
{
    // Nothing to drive until initialize() has run, and nothing to read if no
    // controller is attached. The original code did neither check and polled
    // the joystick every frame regardless.
    if (!m_PUC || !m_PTC || !Joystick::isConnected(0))
    {
        return;
    }

    float deadZone = 10.0f;
    float x = Joystick::getAxisPosition(0, sf::Joystick::X);
    float y = Joystick::getAxisPosition(0, sf::Joystick::Y);

    if (x < deadZone && x > -deadZone)
    {
        x = 0;
    }

    if (y < deadZone && y > -deadZone)
    {
        y = 0;
    }

    m_PUC->updateShipTravelWithController(x, y);

    // Fire on the press, not on every frame the button is held down.
    //
    // This read `isButtonPressed(0, 1) && mButtonPressed`, where mButtonPressed
    // started false and was only ever assigned false -- nothing set it true, so
    // the branch was unreachable and gamepad firing never worked at all. The
    // intent was clearly a debounce; the condition was inverted and the
    // button-release case was missing.
    //
    // Tracking the previous state and firing only on the false -> true
    // transition gives one shot per press. Holding the button does nothing
    // further until it is released.
    const bool fireButtonDown = Joystick::isButtonPressed(0, 1);

    if (fireButtonDown && !m_FireButtonWasDown)
    {
        getPointerToScreenManagerRemoteControl()->shareSoundPlayer().playShoot();
        Vector2f spawnLocation;
        spawnLocation.x = m_PTC->getLocation().x + m_PTC->getSize().x / 2;
        spawnLocation.y = m_PTC->getLocation().y;
        static_cast<GameScreen*>(getParentScreen())->getBulletSpawner()->spawnBullet(spawnLocation, true);
    }

    m_FireButtonWasDown = fireButtonDown;
}

void GameInputHandler::handleKeyPressed(Event& event, RenderWindow&)
{
    // Handle key presses
    if (event.key.code == Keyboard::Escape)
    {
        getPointerToScreenManagerRemoteControl()->shareSoundPlayer().playClick();
        getPointerToScreenManagerRemoteControl()->switchScreens("Select");
        return;
    }

    if (!m_PUC)
    {
        return;
    }

    if (event.key.code == Keyboard::Left)
    {
        m_PUC->moveLeft();
    }

    if (event.key.code == Keyboard::Right)
    {
        m_PUC->moveRight();
    }

    if (event.key.code == Keyboard::Up)
    {
        m_PUC->moveUp();
    }

    if (event.key.code == Keyboard::Down)
    {
        m_PUC->moveDown();
    }
}

void GameInputHandler::handleKeyReleased(Event& event, RenderWindow&)
{
    if (!m_PUC || !m_PTC)
    {
        return;
    }

    if (event.key.code == Keyboard::Left)
    {
        m_PUC->stopLeft();
    }
    else if (event.key.code == Keyboard::Right)
    {
        m_PUC->stopRight();
    }
    else if (event.key.code == Keyboard::Up)
    {
        m_PUC->stopUp();
    }
    else if (event.key.code == Keyboard::Down)
    {
        m_PUC->stopDown();
    }
    else if (event.key.code == Keyboard::Space)
    {
        // Shoot a bullet
        getPointerToScreenManagerRemoteControl()->shareSoundPlayer().playShoot();
        Vector2f spawnLocation;
        spawnLocation.x = m_PTC->getLocation().x + m_PTC->getSize().x / 2;
        spawnLocation.y = m_PTC->getLocation().y;
        static_cast<GameScreen*>(getParentScreen())->spawnBullet(spawnLocation, true);
    }
}