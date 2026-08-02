#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

// Picks the largest character size whose text still fits a given width.
//
// The measurement is injected rather than performed here, so the arithmetic can
// be exercised without a font, a window, or a GL context -- the same move as
// PhysicsEnginePlayMode taking a SoundPlayer instead of reaching for the
// SoundEngine singleton. UIPanel supplies the real measurer, backed by
// sf::Text::getLocalBounds; the tests supply a fake one.
namespace TextFit
{
    // measureWidth(size) must return the rendered width, in pixels, of the text
    // at that character size.
    //
    // Returns the largest size in [minSize, preferredSize] that fits within
    // maxWidth, or minSize if even the smallest overflows -- clipped text is
    // never the better answer, but neither is no text at all.
    //
    // Throws std::invalid_argument if maxWidth is not positive, if minSize is
    // zero, if minSize exceeds preferredSize, or if measureWidth is empty.
    unsigned int largestSizeThatFits(
        float maxWidth,
        unsigned int preferredSize,
        unsigned int minSize,
        const std::function<float(unsigned int)>& measureWidth
    );

    // largestSizeThatFits wired to a real sf::Text, which is left at the size
    // chosen. Set the string and the font first -- both change the measurement.
    //
    // Unlike the function above this one tolerates preferredSize < minSize, by
    // raising the preference: a panel or button too small to hold even the
    // minimum is a legitimate runtime state on a small window, and tiny text
    // beats a thrown exception mid-frame.
    void fitToWidth(
        sf::Text& text,
        float maxWidth,
        unsigned int preferredSize,
        unsigned int minSize = 12
    );

    // Caps a design-time character size by the vertical space left for it.
    //
    // Width is the binding constraint on a wide target, but on a short one
    // nothing else would force a shrink and the text would run off the bottom
    // edge instead. A character size is a nominal em, not a measured height --
    // a line of Roboto-Bold reaches roughly its character size below the pen
    // position once descenders count -- so 0.9 of the space left leaves a
    // margin without a second measuring pass.
    unsigned int sizeForHeight(unsigned int designSize, float availableHeight);
}
