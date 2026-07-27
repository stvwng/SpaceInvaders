#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Loads each font once and hands out references to it.
//
// Button and UIPanel each held an `sf::Font m_Font` **by value** and called
// loadFromFile("fonts/Roboto-Bold.ttf") in their constructors -- nine reads of
// the same file for six buttons and three panels.
//
// The bigger problem was lifetime, not waste. sf::Text does not copy the font
// it is given; it stores a pointer to it. A Font living as a by-value member
// therefore has to outlive every Text that uses it, and copying the owner
// leaves the copy's Text pointing at the original's Font. Centralising
// ownership here means the reference handed out is stable for the life of the
// program.
//
// Note this is still global state, just the safe kind: a function-local static
// is initialised on first use and is not exposed to the static initialisation
// order problem the way the old `static BitmapStore* m_s_Instance` was. Full
// dependency injection would be better on principle, but fonts and textures are
// touched only during construction -- never per frame, and never by a test --
// so it would buy nothing a test could use.
class FontStore
{
    public:
        // Throws std::runtime_error if the file cannot be loaded.
        static const sf::Font& get(const std::string& filename);

        // See BitmapStore::clear() -- same reasoning.
        static void clear();
};
