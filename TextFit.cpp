#include "TextFit.h"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace
{
    [[noreturn]] void reject(const std::string& detail)
    {
        throw std::invalid_argument("TextFit::largestSizeThatFits: " + detail);
    }
}

unsigned int TextFit::largestSizeThatFits(
    float maxWidth,
    unsigned int preferredSize,
    unsigned int minSize,
    const std::function<float(unsigned int)>& measureWidth
)
{
    // Written as !(x > 0) rather than (x <= 0) so a NaN width is rejected too,
    // which is what a panel sized by a division on an unset resolution produces.
    if (!(maxWidth > 0.f))
    {
        reject("maxWidth must be positive, got " + std::to_string(maxWidth));
    }
    if (minSize == 0)
    {
        reject("minSize must be at least 1");
    }
    if (minSize > preferredSize)
    {
        reject(
            "minSize (" + std::to_string(minSize) + ") exceeds preferredSize ("
            + std::to_string(preferredSize) + ")"
        );
    }
    if (!measureWidth)
    {
        reject("measureWidth must be callable");
    }

    const float preferredWidth = measureWidth(preferredSize);
    if (preferredWidth <= maxWidth)
    {
        return preferredSize;
    }

    // Glyph advances scale close to linearly with character size, so one
    // proportional step lands within a size or two of the answer. Estimating
    // beats scanning down from preferredSize: a 160px title shrinking to 90
    // would otherwise cost seventy measurements, each of which rasterises a
    // glyph set the font has not cached yet.
    unsigned int size = static_cast<unsigned int>(
        static_cast<float>(preferredSize) * (maxWidth / preferredWidth)
    );
    if (size < minSize) { size = minSize; }
    if (size > preferredSize) { size = preferredSize; }

    // Hinting rounds every glyph independently, so the estimate is not exact in
    // either direction. Settle it by walking: down while it still overflows...
    while (size > minSize && measureWidth(size) > maxWidth)
    {
        --size;
    }

    // ...then up while the next size up would also fit, so the result is the
    // largest size that fits and not merely some size that fits.
    while (size < preferredSize && measureWidth(size + 1) <= maxWidth)
    {
        ++size;
    }

    return size;
}

void TextFit::fitToWidth(
    sf::Text& text,
    float maxWidth,
    unsigned int preferredSize,
    unsigned int minSize
)
{
    const unsigned int fitted = largestSizeThatFits(
        maxWidth,
        std::max(preferredSize, minSize),
        minSize,
        [&text](unsigned int size)
        {
            text.setCharacterSize(size);
            const sf::FloatRect bounds = text.getLocalBounds();

            // `left` is the first glyph's bearing and is not zero, so the run
            // occupies left + width from the text's position. Counting it makes
            // this measure right for a left-aligned layout and a pixel or two
            // pessimistic for a centred one, which is the safe direction.
            return bounds.left + bounds.width;
        }
    );

    text.setCharacterSize(fitted);
}

unsigned int TextFit::sizeForHeight(unsigned int designSize, float availableHeight)
{
    // Clamp before the cast: converting a negative float to unsigned is
    // undefined, and text starting below its own container's bottom edge is
    // exactly the case this guard exists for.
    const float usable = std::max(0.f, availableHeight * 0.9f);
    return std::min(designSize, static_cast<unsigned int>(usable));
}
