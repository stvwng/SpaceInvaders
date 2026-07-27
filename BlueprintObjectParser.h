#pragma once
#include "GameObjectBlueprint.h"
#include <string>
#include <fstream>

using namespace std;

class BlueprintObjectParser
{
    public:
        void parseNextObjectForBlueprint(ifstream& reader, GameObjectBlueprint& bp);

        // Returns the text between startTag and endTag, trimmed of surrounding
        // whitespace, or "" if either tag is absent.
        // Public so it can be tested directly -- it is where the subtlest
        // parsing bug in this project lived.
        string extractStringBetweenTags(
            const string& stringToSearch,
            const string& startTag,
            const string& endTag
        );
};
