#include <doctest/doctest.h>
#include "BlueprintObjectParser.h"
#include "ObjectTags.h"

// Regression tests for the tag extractor. The original implementation sliced by
// tag *length* rather than searching:
//
//     start = startTag.length();
//     count = line.length() - startTag.length() - endTag.length();
//
// which is correct only when the opening tag starts at column 0 and the closing
// tag is exactly the length the code assumed.

TEST_CASE("extractStringBetweenTags reads a well-formed line")
{
    BlueprintObjectParser parser;
    CHECK(parser.extractStringBetweenTags("[NAME]invader[-NAME]", "[NAME]", "[-NAME]") == "invader");
}

TEST_CASE("extractStringBetweenTags does not depend on the closing tag's length")
{
    BlueprintObjectParser parser;

    // This is the shape of the bug that hid the world/level1 collider typo:
    // "[-ENCOMPASSING_RECT COLLIDER]" and "[-ENCOMPASSING RECT COLLIDER]" are
    // the same length, so a length-based slice produced the right answer for
    // the wrong reason. A closing tag of a *different* length exposes it.
    CHECK(parser.extractStringBetweenTags("[NAME]invader[-N]", "[NAME]", "[-N]") == "invader");
    CHECK(parser.extractStringBetweenTags(
              "[NAME]invader[-A VERY LONG CLOSING TAG]", "[NAME]", "[-A VERY LONG CLOSING TAG]") == "invader");
}

TEST_CASE("extractStringBetweenTags tolerates leading text and indentation")
{
    BlueprintObjectParser parser;
    CHECK(parser.extractStringBetweenTags("    [WIDTH]2[-WIDTH]", "[WIDTH]", "[-WIDTH]") == "2");
}

TEST_CASE("extractStringBetweenTags strips a CRLF carriage return")
{
    BlueprintObjectParser parser;

    // getline leaves the "\r" behind on a file checked out with Windows line
    // endings. The old code passed it straight to stof, which throws.
    CHECK(parser.extractStringBetweenTags("[WIDTH]2[-WIDTH]\r", "[WIDTH]", "[-WIDTH]") == "2");
    CHECK(parser.extractStringBetweenTags("[NAME] invader \t[-NAME]", "[NAME]", "[-NAME]") == "invader");
}

TEST_CASE("extractStringBetweenTags returns empty when a tag is missing")
{
    BlueprintObjectParser parser;

    // The real failure mode: the declared closing tag does not appear in the
    // data at all. Returning "" makes that visible; the old code returned a
    // silently truncated string instead.
    CHECK(parser.extractStringBetweenTags("[COMPONENT]Standard Graphics[-COMPONENT]",
                                          "[COMPONENT]", "[-END COMPONENT]") == "");
    CHECK(parser.extractStringBetweenTags("no tags here at all", "[NAME]", "[-NAME]") == "");
    CHECK(parser.extractStringBetweenTags("[NAME][-NAME]", "[NAME]", "[-NAME]") == "");
}

TEST_CASE("the component tags actually agree with the level data")
{
    BlueprintObjectParser parser;

    // This is the test that would have caught the biggest bug in the project.
    // ObjectTags declared "[-END COMPONENT]" while every level file writes
    // "[-COMPONENT]", so no component name was ever extracted and every
    // GameObject was built with no components at all.
    const std::string realLine = "[COMPONENT]Standard Graphics[-COMPONENT]";
    CHECK(parser.extractStringBetweenTags(realLine, ObjectTags::COMPONENT, ObjectTags::COMPONENT_END)
          == "Standard Graphics");
}
