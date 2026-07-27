#include <doctest/doctest.h>
#include "ObjectTags.h"
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

// The code and the level data are two halves of one format, and nothing forced
// them to agree. Two mismatches had gone unnoticed:
//
//   * ObjectTags said the component block closed with "[-END COMPONENT]";
//     every level file writes "[-COMPONENT]". No component was ever parsed.
//   * world/level1 closed its collider block with an underscore,
//     "[-ENCOMPASSING_RECT COLLIDER]", which no declared tag matches.
//
// These tests read the real level file and check the two halves still line up,
// so a future edit to either side cannot drift again.

namespace
{
    std::vector<std::string> readLevelFile()
    {
        std::ifstream reader(std::string(SPACEINVADERS_REPO_ROOT) + "/world/level1");
        REQUIRE_MESSAGE(reader.is_open(), "could not open world/level1");

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(reader, line))
        {
            lines.push_back(line);
        }
        return lines;
    }
}

TEST_CASE("every opening tag in world/level1 is closed by its declared tag")
{
    const std::vector<std::pair<std::string, std::string>> tagPairs = {
        {ObjectTags::COMPONENT, ObjectTags::COMPONENT_END},
        {ObjectTags::NAME, ObjectTags::NAME_END},
        {ObjectTags::WIDTH, ObjectTags::WIDTH_END},
        {ObjectTags::HEIGHT, ObjectTags::HEIGHT_END},
        {ObjectTags::LOCATION_X, ObjectTags::LOCATION_X_END},
        {ObjectTags::LOCATION_Y, ObjectTags::LOCATION_Y_END},
        {ObjectTags::BITMAP_NAME, ObjectTags::BITMAP_NAME_END},
        {ObjectTags::SPEED, ObjectTags::SPEED_END},
        {ObjectTags::ENCOMPASSING_RECT_COLLIDER, ObjectTags::ENCOMPASSING_RECT_COLLIDER_END},
    };

    const std::vector<std::string> lines = readLevelFile();

    for (const auto& pair : tagPairs)
    {
        const std::string& openTag = pair.first;
        const std::string& closeTag = pair.second;

        size_t linesWithOpenTag = 0;
        size_t linesAlsoClosed = 0;

        for (const std::string& line : lines)
        {
            // NAME would otherwise match inside BITMAP NAME.
            if (line.find(openTag) == std::string::npos) { continue; }
            if (openTag == ObjectTags::NAME && line.find(ObjectTags::BITMAP_NAME) != std::string::npos) { continue; }

            linesWithOpenTag++;
            if (line.find(closeTag) != std::string::npos)
            {
                linesAlsoClosed++;
            }
        }

        INFO("tag pair: " << openTag << " ... " << closeTag);
        CHECK(linesWithOpenTag == linesAlsoClosed);
    }
}

TEST_CASE("world/level1 declares the objects the game expects")
{
    const std::vector<std::string> lines = readLevelFile();

    size_t objects = 0, invaders = 0, bullets = 0, players = 0;
    for (const std::string& line : lines)
    {
        if (line.find(ObjectTags::START_OF_OBJECT) != std::string::npos) { objects++; }
        if (line.find("[NAME]invader[-NAME]") != std::string::npos) { invaders++; }
        if (line.find("[NAME]bullet[-NAME]") != std::string::npos) { bullets++; }
        if (line.find("[NAME]Player[-NAME]") != std::string::npos) { players++; }
    }

    CHECK(objects == 60);
    CHECK(invaders == 45);
    CHECK(bullets == 14);

    // PhysicsEnginePlayMode and GameInputHandler both resolve the player by
    // this exact tag and now throw if it is absent.
    CHECK(players == 1);
}

TEST_CASE("every component named in world/level1 is one the factory can build")
{
    // GameObjectFactoryPlayMode matches these strings exactly. A typo in the
    // data silently produces an object missing that component -- which is how
    // the "[-END COMPONENT]" bug stayed invisible for so long.
    const std::vector<std::string> buildable = {
        "Transform", "Player Update", "Invader Update", "Bullet Update", "Standard Graphics"
    };

    const std::vector<std::string> lines = readLevelFile();
    size_t componentLines = 0;

    for (const std::string& line : lines)
    {
        const size_t open = line.find(ObjectTags::COMPONENT);
        if (open == std::string::npos) { continue; }

        const size_t valueStart = open + ObjectTags::COMPONENT.length();
        const size_t close = line.find(ObjectTags::COMPONENT_END, valueStart);
        REQUIRE(close != std::string::npos);

        const std::string name = line.substr(valueStart, close - valueStart);
        componentLines++;

        INFO("component name in level file: \"" << name << "\"");
        CHECK(std::find(buildable.begin(), buildable.end(), name) != buildable.end());
    }

    CHECK(componentLines > 0);
}
