#include "BitmapStore.h"
#include <map>
#include <stdexcept>

namespace
{
    std::map<std::string, sf::Texture>& textures()
    {
        static std::map<std::string, sf::Texture> loaded;
        return loaded;
    }
}

void BitmapStore::clear()
{
    textures().clear();
}

const sf::Texture& BitmapStore::getBitmap(const std::string& filename)
{
    auto& loaded = textures();

    auto found = loaded.find(filename);
    if (found != loaded.end())
    {
        return found->second;
    }

    auto& texture = loaded[filename];
    if (!texture.loadFromFile(filename))
    {
        // Erase before throwing: leaving an empty texture in the map would let
        // a later call succeed with a 0x0 image, and StandardGraphicsComponent
        // divides the object size by the texture size to compute a scale.
        loaded.erase(filename);
        throw std::runtime_error("BitmapStore::getBitmap - could not load \"" + filename + "\"");
    }

    return texture;
}
