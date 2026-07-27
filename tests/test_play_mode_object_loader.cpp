#include <doctest/doctest.h>
#include "PlayModeObjectLoader.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <string>
#include <vector>
#include <stdexcept>

// End-to-end over the load path: file -> parser -> blueprint -> factory ->
// GameObject. Every fixture used here declares only a Transform, so nothing in
// these tests loads a texture, opens a window, or plays a sound.

namespace
{
    std::string fixture(const std::string& name)
    {
        return std::string(SPACEINVADERS_TEST_FIXTURES) + "/" + name;
    }
}

TEST_CASE("a level's values reach the built GameObject intact")
{
    PlayModeObjectLoader loader;
    std::vector<GameObject> objects;

    loader.loadGameObjectsForPlayMode(fixture("minimal_level"), objects);

    REQUIRE(objects.size() == 1);
    CHECK(objects[0].getTagName() == "testobject");

    auto transform = objects[0].getTransformComponent();

    // The setLocationX/setLocationY bug wrote both of these into m_Height, so
    // this is the assertion that pins it down end-to-end.
    CHECK(transform->getLocation().x == doctest::Approx(12.5f));
    CHECK(transform->getLocation().y == doctest::Approx(7.25f));
    CHECK(transform->getSize().x == doctest::Approx(3.f));
    CHECK(transform->getSize().y == doctest::Approx(4.f));

    CHECK(objects[0].hasCollider());
    CHECK(objects[0].getEncompassingRectColliderTag() == "testcollider");
}

TEST_CASE("a level with Windows line endings loads identically")
{
    PlayModeObjectLoader loader;
    std::vector<GameObject> objects;

    // getline leaves the "\r" on the end of every value. Before the extractor
    // trimmed whitespace this reached stof and threw std::invalid_argument,
    // which nothing caught.
    loader.loadGameObjectsForPlayMode(fixture("crlf_level"), objects);

    REQUIRE(objects.size() == 1);
    CHECK(objects[0].getTagName() == "testobject");

    auto transform = objects[0].getTransformComponent();
    CHECK(transform->getLocation().x == doctest::Approx(12.5f));
    CHECK(transform->getLocation().y == doctest::Approx(7.25f));
    CHECK(objects[0].getEncompassingRectColliderTag() == "testcollider");
}

TEST_CASE("objects accumulate in file order and keep distinct positions")
{
    PlayModeObjectLoader loader;
    std::vector<GameObject> objects;

    loader.loadGameObjectsForPlayMode(fixture("two_object_level"), objects);

    REQUIRE(objects.size() == 2);
    CHECK(objects[0].getTagName() == "first");
    CHECK(objects[1].getTagName() == "second");

    CHECK(objects[0].getTransformComponent()->getLocation().x == doctest::Approx(1.f));
    CHECK(objects[1].getTransformComponent()->getLocation().x == doctest::Approx(30.f));
    CHECK(objects[0].getTransformComponent()->getSize().y == doctest::Approx(6.f));
    CHECK(objects[1].getTransformComponent()->getSize().y == doctest::Approx(8.f));
}

TEST_CASE("a missing level file is reported, not ignored")
{
    PlayModeObjectLoader loader;
    std::vector<GameObject> objects;

    // The ifstream state used to go unchecked, so a missing file produced zero
    // objects and the next step -- finding the object tagged "Player" -- read
    // off the end of an empty vector.
    CHECK_THROWS_AS(
        loader.loadGameObjectsForPlayMode(fixture("does_not_exist"), objects),
        std::runtime_error
    );
    CHECK(objects.empty());
}

TEST_CASE("an empty level file yields no objects and does not throw")
{
    PlayModeObjectLoader loader;
    std::vector<GameObject> objects;

    loader.loadGameObjectsForPlayMode(fixture("empty_level"), objects);
    CHECK(objects.empty());
}

TEST_CASE("asking a component-less object for a component throws")
{
    GameObject bare;
    bare.setTag("bare");

    // These accessors index m_Components with a cached position that is -1
    // until the component is added. Indexing a vector with -1 is undefined
    // behaviour, and ASan caught it happening for real once the component tag
    // mismatch stopped every component from being built.
    CHECK_THROWS_AS(bare.getTransformComponent(), std::runtime_error);
    CHECK_THROWS_AS(bare.getGraphicsComponent(), std::runtime_error);
    CHECK_THROWS_AS(bare.getFirstUpdateComponent(), std::runtime_error);
    CHECK_FALSE(bare.hasCollider());
}
