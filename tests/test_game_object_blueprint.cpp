#include <doctest/doctest.h>
#include "GameObjectBlueprint.h"

// setLocationX and setLocationY both assigned to m_Height. Every object in
// every level therefore drew its position from an uninitialised member, and its
// height from whichever location was written last. A round-trip test over the
// setters is the whole fix -- and it is one line per property.

TEST_CASE("each blueprint setter writes its own field")
{
    GameObjectBlueprint bp;

    bp.setWidth(3.f);
    bp.setHeight(4.f);
    bp.setLocationX(12.5f);
    bp.setLocationY(7.25f);
    bp.setSpeed(42.5f);

    CHECK(bp.getWidth() == doctest::Approx(3.f));
    CHECK(bp.getHeight() == doctest::Approx(4.f));
    CHECK(bp.getLocationX() == doctest::Approx(12.5f));
    CHECK(bp.getLocationY() == doctest::Approx(7.25f));
    CHECK(bp.getSpeed() == doctest::Approx(42.5f));
}

TEST_CASE("setting a location does not disturb the size")
{
    GameObjectBlueprint bp;

    bp.setWidth(3.f);
    bp.setHeight(4.f);
    bp.setLocationX(99.f);
    bp.setLocationY(88.f);

    // The original bug, stated directly.
    CHECK(bp.getHeight() == doctest::Approx(4.f));
    CHECK(bp.getWidth() == doctest::Approx(3.f));
}

TEST_CASE("a default blueprint has defined numeric fields")
{
    GameObjectBlueprint bp;

    // These had no initialisers, so a level file omitting a tag left them
    // indeterminate.
    CHECK(bp.getWidth() == doctest::Approx(0.f));
    CHECK(bp.getHeight() == doctest::Approx(0.f));
    CHECK(bp.getLocationX() == doctest::Approx(0.f));
    CHECK(bp.getLocationY() == doctest::Approx(0.f));
    CHECK(bp.getSpeed() == doctest::Approx(0.f));
}

TEST_CASE("name, bitmap and component list round-trip")
{
    GameObjectBlueprint bp;

    CHECK(bp.getName() == "");
    CHECK(bp.getEncompassingRectCollider() == false);

    bp.setName("invader");
    bp.setBitmapName("invader1");
    bp.addToComponentList("Transform");
    bp.addToComponentList("Standard Graphics");
    bp.setEncompassingRectCollider("invader");

    CHECK(bp.getName() == "invader");
    CHECK(bp.getBitmapName() == "invader1");
    CHECK(bp.getComponentList().size() == 2);
    CHECK(bp.getComponentList()[0] == "Transform");
    CHECK(bp.getComponentList()[1] == "Standard Graphics");
    CHECK(bp.getEncompassingRectCollider() == true);
    CHECK(bp.getEncompassingRectColliderLabel() == "invader");
}
