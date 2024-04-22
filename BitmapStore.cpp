#include "BitmapStore.h"
#include <assert.h>

using namespace sf;

BitmapStore* BitmapStore::m_s_Instance = nullptr;

BitmapStore::BitmapStore()
{
    assert(m_s_Instance == nullptr);
    m_s_Instance = this;
}

void BitmapStore::addBitmap(std::string const& filename)
{
    // Get reference to m_Textures using m_s_Instance
    auto& bitmapsMap = m_s_Instance->m_BitmapsMap;

    // Create an iterator to hold a key-value pair
    // And search using the filename
    auto keyValuePair = bitmapsMap.find(filename);
    // auto is the equivalent of map<string, Texture>::iterator here

    // No match is found
    if (keyValuePair == bitmapsMap.end())
    {
        auto& texture = bitmapsMap[filename];
        texture.loadFromFile(filename);
    }
}

sf::Texture& BitmapStore::getBitmap(std::string const& filename)
{
    // Get reference to m_Textures using m_s_Instance
    auto& bitmapsMap = m_s_Instance->m_BitmapsMap;

    // Create an iterator to hold a key-value pair
    // And search using the filename
    auto keyValuePair = bitmapsMap.find(filename);
    // auto is the equivalent of map<string, Texture>::iterator here

    // Did we find a match?
    if (keyValuePair != bitmapsMap.end())
    {
        return keyValuePair->second;
    }
    else
    {
        #ifdef debuggingOnConsole
        cout << "BitmapStore::getBitmap()Texture not found" << endl;
        #endif
        return keyValuePair->second;
    }
}