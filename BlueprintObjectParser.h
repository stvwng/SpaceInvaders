#pragma once
#include "GameObjectBlueprint.h"
#include <string>
#include <fstream>


class BlueprintObjectParser
{
    public:
        void parseNextObjectForBlueprint(std::ifstream& reader, GameObjectBlueprint& bp);

        // Returns the text between startTag and endTag, trimmed of surrounding
        // whitespace, or "" if either tag is absent.
        // Public so it can be tested directly -- it is where the subtlest
        // parsing bug in this project lived.
        std::string extractStringBetweenTags(
            const std::string& stringToSearch,
            const std::string& startTag,
            const std::string& endTag
        );
};
