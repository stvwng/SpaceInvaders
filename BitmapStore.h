#pragma once
#ifndef BITMAP_STORE_H
#define BITMAP_STORE_H

#include <SFML/Graphics.hpp>
#include <string>

// Loads each texture once and hands out references to it.
//
// This used to be a hand-rolled singleton: a `static BitmapStore* m_s_Instance`
// assigned in the constructor and guarded by assert(), with ScreenManager
// holding a `BitmapStore m_BS` member whose only purpose was to make that
// assignment happen. Every static member function then dereferenced the raw
// pointer, so the class silently depended on someone, somewhere, having
// constructed one first -- and on that instance outliving every user.
//
// The map now lives in a function-local static: initialised on first use,
// thread-safe since C++11, with no instance to construct and no ordering to get
// wrong. See FontStore.h for why this is not full dependency injection.
class BitmapStore
{
    public:
        // Loads on first request. Throws std::runtime_error if the file cannot
        // be read -- previously the failure was discarded and produced a 0x0
        // texture, which callers then divided by to compute a sprite scale.
        static const sf::Texture& getBitmap(const std::string& filename);

        // Release every texture. Must be called while the RenderWindow, and
        // therefore the OpenGL context, is still alive: a function-local
        // static is destroyed after main() returns, by which point
        // ~Texture has no context to free its handle against.
        static void clear();
};

#endif
