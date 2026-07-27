#include "BitmapStore.h"
#include "DevelopState.h"
#include <assert.h>
#include <iostream>
#include <stdexcept>

using namespace sf;

BitmapStore* BitmapStore::m_s_Instance = nullptr;

BitmapStore::BitmapStore()
{
    assert(m_s_Instance == nullptr);
    m_s_Instance = this;
}

void BitmapStore::addBitmap(std::string const& filename)
{
    auto& bitmapsMap = m_s_Instance->m_BitmapsMap;

    // Already loaded? Nothing to do.
    if (bitmapsMap.find(filename) != bitmapsMap.end())
    {
        return;
    }

    auto& texture = bitmapsMap[filename];

    // loadFromFile returns a bool that this code used to discard. A missing
    // file then produced a 0x0 texture, and StandardGraphicsComponent divided
    // the object size by that zero to compute a scale -- so a typo in an asset
    // name surfaced as invisible sprites or NaN positions rather than as an
    // error.
    if (!texture.loadFromFile(filename))
    {
        bitmapsMap.erase(filename);
        throw std::runtime_error("BitmapStore::addBitmap - could not load \"" + filename + "\"");
    }
}

sf::Texture& BitmapStore::getBitmap(std::string const& filename)
{
    auto& bitmapsMap = m_s_Instance->m_BitmapsMap;

    auto keyValuePair = bitmapsMap.find(filename);
    if (keyValuePair != bitmapsMap.end())
    {
        return keyValuePair->second;
    }

    #ifdef SPACEINVADERS_DEBUG_LOG
    std::cout << "BitmapStore::getBitmap() texture not found: " << filename << std::endl;
    #endif

    // The not-found branch used to `return keyValuePair->second` -- that is,
    // dereference the end() iterator it had just compared against end().
    throw std::runtime_error("BitmapStore::getBitmap - no texture loaded for \"" + filename + "\"");
}
