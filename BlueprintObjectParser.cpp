#include "BlueprintObjectParser.h"
#include "ObjectTags.h"
#include "DevelopState.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace std;

namespace
{
    // stof throws on malformed input. A level file is data the game does not
    // control, so a bad value must not terminate the process.
    bool tryParseFloat(const string& text, float& out)
    {
        try
        {
            size_t charactersConsumed = 0;
            const float parsed = std::stof(text, &charactersConsumed);
            if (charactersConsumed != text.length())
            {
                return false; // trailing junk, e.g. "12abc"
            }
            out = parsed;
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    void warnBadNumber(const string& tag, const string& value)
    {
        #ifdef SPACEINVADERS_DEBUG_LOG
        std::cout << "BlueprintObjectParser: could not parse a number from "
                  << tag << " -> \"" << value << "\"" << std::endl;
        #else
        (void)tag;
        (void)value;
        #endif
    }
}

void BlueprintObjectParser::parseNextObjectForBlueprint(ifstream& reader, GameObjectBlueprint& bp)
{
    string lineFromFile;
    string value = "";
    float number = 0.f;

    while (getline(reader, lineFromFile))
    {
        if (lineFromFile.find(ObjectTags::COMPONENT) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::COMPONENT, ObjectTags::COMPONENT_END);
            bp.addToComponentList(value);
        }
        else if (lineFromFile.find(ObjectTags::NAME) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::NAME, ObjectTags::NAME_END);
            bp.setName(value);
        }
        else if (lineFromFile.find(ObjectTags::WIDTH) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::WIDTH, ObjectTags::WIDTH_END);
            if (tryParseFloat(value, number)) { bp.setWidth(number); }
            else { warnBadNumber(ObjectTags::WIDTH, value); }
        }
        else if (lineFromFile.find(ObjectTags::HEIGHT) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::HEIGHT, ObjectTags::HEIGHT_END);
            if (tryParseFloat(value, number)) { bp.setHeight(number); }
            else { warnBadNumber(ObjectTags::HEIGHT, value); }
        }
        else if (lineFromFile.find(ObjectTags::LOCATION_X) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::LOCATION_X, ObjectTags::LOCATION_X_END);
            if (tryParseFloat(value, number)) { bp.setLocationX(number); }
            else { warnBadNumber(ObjectTags::LOCATION_X, value); }
        }
        else if (lineFromFile.find(ObjectTags::LOCATION_Y) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::LOCATION_Y, ObjectTags::LOCATION_Y_END);
            if (tryParseFloat(value, number)) { bp.setLocationY(number); }
            else { warnBadNumber(ObjectTags::LOCATION_Y, value); }
        }
        else if (lineFromFile.find(ObjectTags::SPEED) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::SPEED, ObjectTags::SPEED_END);
            if (tryParseFloat(value, number)) { bp.setSpeed(number); }
            else { warnBadNumber(ObjectTags::SPEED, value); }
        }
        else if (lineFromFile.find(ObjectTags::BITMAP_NAME) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::BITMAP_NAME, ObjectTags::BITMAP_NAME_END);
            bp.setBitmapName(value);
        }
        else if (lineFromFile.find(ObjectTags::ENCOMPASSING_RECT_COLLIDER) != string::npos)
        {
            value = extractStringBetweenTags(lineFromFile, ObjectTags::ENCOMPASSING_RECT_COLLIDER, ObjectTags::ENCOMPASSING_RECT_COLLIDER_END);
            bp.setEncompassingRectCollider(value);
        }
        else if (lineFromFile.find(ObjectTags::END_OF_OBJECT) != string::npos)
        {
            return;
        }
    }
}

string BlueprintObjectParser::extractStringBetweenTags(
    const string& stringToSearch,
    const string& startTag,
    const string& endTag
)
{
    // This used to slice by tag *length* rather than by searching for the tags:
    //     start = startTag.length()
    //     count = line.length() - startTag.length() - endTag.length()
    //
    // That produced the right answer only when the opening tag sat at column 0
    // and the closing tag was exactly the expected number of characters.
    // world/level1 closed its collider tag with an underscore
    // ("[-ENCOMPASSING_RECT COLLIDER]") which never matched the declared tag,
    // and the old code did not notice purely because the two spellings happen
    // to be the same length. A trailing "\r" from a Windows checkout would have
    // corrupted every value in the file the same way.
    const size_t startTagPosition = stringToSearch.find(startTag);
    if (startTagPosition == string::npos)
    {
        return "";
    }

    const size_t valueStart = startTagPosition + startTag.length();

    const size_t endTagPosition = stringToSearch.find(endTag, valueStart);
    if (endTagPosition == string::npos)
    {
        return "";
    }

    string extracted = stringToSearch.substr(valueStart, endTagPosition - valueStart);

    // Trim surrounding whitespace, including the "\r" left by CRLF line endings.
    const char* whitespace = " \t\r\n";
    const size_t firstReal = extracted.find_first_not_of(whitespace);
    if (firstReal == string::npos)
    {
        return "";
    }
    const size_t lastReal = extracted.find_last_not_of(whitespace);

    return extracted.substr(firstReal, lastReal - firstReal + 1);
}
