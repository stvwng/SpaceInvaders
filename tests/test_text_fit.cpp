#include <doctest/doctest.h>
#include "TextFit.h"

#include <functional>
#include <limits>
#include <stdexcept>

// The title screen rendered "SPACE INVA" because SelectUIPanel asked for a
// 160px character size inside a panel sized as a fraction of the display, and
// SFML clips drawing to the panel's viewport. The sizing decision now lives in
// largestSizeThatFits, which takes its measurements from a callback -- so these
// cases pin the arithmetic without loading a font or opening a window.

namespace
{
    // Advance width scales linearly with character size, which is very close to
    // how a real font behaves before hinting rounds each glyph.
    std::function<float(unsigned int)> linearFont(float widthPerSizeUnit)
    {
        return [widthPerSizeUnit](unsigned int size)
        {
            return static_cast<float>(size) * widthPerSizeUnit;
        };
    }
}

TEST_CASE("a size that already fits is returned untouched")
{
    // 160 * 8 = 1280, comfortably inside 2000.
    CHECK(TextFit::largestSizeThatFits(2000.f, 160, 12, linearFont(8.f)) == 160);
}

TEST_CASE("an oversized preference is shrunk until it fits")
{
    // The real bug, in miniature: 160 * 8 = 1280px of text in a 900px panel.
    const unsigned int fitted =
        TextFit::largestSizeThatFits(900.f, 160, 12, linearFont(8.f));

    CHECK(fitted < 160);
    CHECK(static_cast<float>(fitted) * 8.f <= 900.f);
}

TEST_CASE("the result is the largest size that fits, not merely one that fits")
{
    const float maxWidth = 900.f;
    const auto measure = linearFont(8.f);

    const unsigned int fitted =
        TextFit::largestSizeThatFits(maxWidth, 160, 12, measure);

    CHECK(measure(fitted) <= maxWidth);
    CHECK(measure(fitted + 1) > maxWidth);   // one size larger overflows
    CHECK(fitted == 112);                    // 112 * 8 = 896, 113 * 8 = 904
}

TEST_CASE("a step-quantised font is still fitted exactly")
{
    // Hinting rounds each glyph independently, so measured width is a staircase
    // rather than a line. This is the case the proportional estimate gets wrong
    // on its own -- it exists to prove the walk afterwards corrects it.
    const auto stepped = [](unsigned int size)
    {
        const unsigned int quantised = (size / 10) * 10;
        return static_cast<float>(quantised) * 8.f;
    };

    const unsigned int fitted =
        TextFit::largestSizeThatFits(900.f, 160, 12, stepped);

    CHECK(stepped(fitted) <= 900.f);
    CHECK(stepped(fitted + 1) > 900.f);
    CHECK(fitted == 119);   // 110*8 = 880 fits; at 120 the step jumps to 960
}

TEST_CASE("an estimate that overshoots is walked back down")
{
    // Width with a fixed overhead that does not scale -- a real sf::Text has
    // one in the first glyph's bearing. Proportional scaling assumes the whole
    // width shrinks, so the estimate lands too large and must be corrected
    // downwards, the opposite direction from the stepped case above.
    const auto withOverhead = [](unsigned int size)
    {
        return static_cast<float>(size) * 8.f + 200.f;
    };

    const unsigned int fitted =
        TextFit::largestSizeThatFits(900.f, 160, 12, withOverhead);

    CHECK(withOverhead(fitted) <= 900.f);
    CHECK(withOverhead(fitted + 1) > 900.f);
    CHECK(fitted == 87);   // 87*8 + 200 = 896; the estimate alone lands on 97
}

TEST_CASE("the minimum wins when even the smallest size overflows")
{
    // A panel far too narrow for the string. Tiny text is the answer; there is
    // no size that fits, and returning zero or throwing would blank the screen.
    CHECK(TextFit::largestSizeThatFits(10.f, 160, 12, linearFont(8.f)) == 12);
}

TEST_CASE("a preferred size equal to the minimum is legal")
{
    CHECK(TextFit::largestSizeThatFits(1000.f, 12, 12, linearFont(8.f)) == 12);
    CHECK(TextFit::largestSizeThatFits(1.f, 12, 12, linearFont(8.f)) == 12);
}

TEST_CASE("nonsensical arguments are rejected rather than guessed at")
{
    const auto measure = linearFont(8.f);

    CHECK_THROWS_AS(
        TextFit::largestSizeThatFits(0.f, 160, 12, measure), std::invalid_argument);
    CHECK_THROWS_AS(
        TextFit::largestSizeThatFits(-100.f, 160, 12, measure), std::invalid_argument);

    // A panel width computed from an unset resolution arrives as NaN, which
    // compares false against every bound and would otherwise sail through.
    CHECK_THROWS_AS(
        TextFit::largestSizeThatFits(
            std::numeric_limits<float>::quiet_NaN(), 160, 12, measure),
        std::invalid_argument);

    // A zero minimum would let the walk return a character size of 0.
    CHECK_THROWS_AS(
        TextFit::largestSizeThatFits(900.f, 160, 0, measure), std::invalid_argument);

    // An inverted range has no correct answer.
    CHECK_THROWS_AS(
        TextFit::largestSizeThatFits(900.f, 12, 160, measure), std::invalid_argument);

    CHECK_THROWS_AS(
        TextFit::largestSizeThatFits(900.f, 160, 12, nullptr), std::invalid_argument);
}
