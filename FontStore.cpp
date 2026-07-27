#include "FontStore.h"
#include <map>
#include <stdexcept>

using namespace std;
using namespace sf;

namespace
{
    // Function-local static: initialised on first use, thread-safe since C++11,
    // and destroyed in a defined order at exit.
    std::map<std::string, sf::Font>& fonts()
    {
        static std::map<std::string, sf::Font> loaded;
        return loaded;
    }
}

void FontStore::clear()
{
    fonts().clear();
}

const sf::Font& FontStore::get(const std::string& filename)
{
    auto& loaded = fonts();

    auto found = loaded.find(filename);
    if (found != loaded.end())
    {
        return found->second;
    }

    auto& font = loaded[filename];
    if (!font.loadFromFile(filename))
    {
        // Erase before throwing, so a later call retries rather than handing
        // back an empty font that silently renders nothing.
        loaded.erase(filename);
        throw std::runtime_error("FontStore::get - could not load \"" + filename + "\"");
    }

    return font;
}
