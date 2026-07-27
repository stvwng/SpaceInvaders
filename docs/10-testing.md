# 10 — Testing C++ with doctest

## Overview

This project had no tests. Several of its worst defects — location setters
writing the wrong field, a tag constant that didn't match the data file — are
exactly the kind a one-line test catches instantly and a human reading the code
misses for years.

The suite added here is 28 cases and 470 assertions, and it is deliberately
constrained: **no test opens a window, loads a texture, or plays a sound.** That
constraint is what makes it runnable in any context, and it's also what forced
the interesting design question — how do you test a game when most of the code
touches a graphics library?

## ELI5

Two ways to check a recipe.

The slow way: cook the entire meal, plate it, taste it. Works, but takes an hour,
needs a kitchen, and if it's wrong you don't know *which* step went wrong.

The fast way: taste the sauce right after you make it. Seconds, no oven, and if
it's too salty you know exactly where the salt went in.

Most of this suite is the second kind. It doesn't launch the game — it takes the
parts that are just *thinking* (reading the level file, doing the arithmetic) and
checks those directly. A few things genuinely need the whole kitchen, and those
stay manual for now.

## For a SWE

### Setup

doctest is a single header, vendored in `third_party/doctest/` rather than
fetched at configure time so the build works offline.

The important structural decision is in `CMakeLists.txt`:

```cmake
# Everything except main() lives in a static library.
file(GLOB SPACEINVADERS_SOURCES CONFIGURE_DEPENDS "*.cpp")
list(REMOVE_ITEM SPACEINVADERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/SpaceInvaders.cpp")
add_library(spaceinvaders_lib STATIC ${SPACEINVADERS_SOURCES})

add_executable(SpaceInvaders SpaceInvaders.cpp)
target_link_libraries(SpaceInvaders PRIVATE spaceinvaders_lib)

add_executable(SpaceInvadersTests ${SPACEINVADERS_TEST_SOURCES})
target_link_libraries(SpaceInvadersTests PRIVATE spaceinvaders_lib doctest)
```

`SpaceInvaders.cpp` contains nothing but `main()`. Everything else is in a library
that both binaries link, so the tests exercise the **real** objects — not a
recompiled copy with different flags. Splitting `main()` out is the single change
that makes a C++ application testable, and it costs four lines.

```bash
cmake --build build --target SpaceInvadersTests
cd build && ./SpaceInvadersTests      # or ctest --test-dir build
```

### What is testable, and what isn't

The suite splits along one line: does this code need a GPU context or an audio
device?

| Testable now | Needs a window/device |
|---|---|
| `BlueprintObjectParser` — pure string manipulation | `StandardGraphicsComponent::draw` |
| `GameObjectBlueprint` — plain data | `Button` / `UIPanel` (fonts, views) |
| `PlayModeObjectLoader` — file I/O + object construction | `GameScreen::draw` |
| `ObjectTags` vs the real level file | `InputHandler` (SFML events) |
| `GameObject` component lookup and error paths | screen transitions |
| **`PhysicsEnginePlayMode` — all collision logic** | |

The right-hand column isn't untestable in principle — it's untestable *as
currently written*, because the logic is entangled with the I/O. Which is the
actual lesson:

> **You don't write tests to check code. You write tests and discover which parts
> of your design were separable all along.**

`PhysicsEnginePlayMode` was the clearest case, and it has since moved columns.
Its collision arithmetic is pure — rectangles, floats, counters — but it called
`SoundEngine::playInvaderExplode()` inline on every hit, and `SoundEngine` was a
singleton reached through a raw `static` pointer. There was no way to substitute
a silent one, so testing collisions meant making noise.

That was never a testing problem. It was the singleton, showing up as a testing
problem — and replacing it with an injected `SoundPlayer`
([primer 08](08-resource-management.md)) moved the highest-risk code in the game
from untestable to eight covered cases:

```cpp
NullSoundPlayer silence;
PhysicsEnginePlayMode physics;
physics.initialize(sharer, silence);       // the whole engine, in silence
```

The lesson generalises: when something is hard to test, the obstacle is usually a
dependency reached through a global rather than passed in. Fix the dependency and
the test writes itself.

### Fixtures over mocks

The load pipeline is tested with small real files in `tests/fixtures/` rather
than mocked streams:

```
tests/fixtures/minimal_level      one object, Transform only
tests/fixtures/two_object_level   two objects, distinct positions
tests/fixtures/crlf_level         identical content, CRLF line endings
tests/fixtures/empty_level        zero bytes
```

Every fixture declares only a `Transform` — no `Standard Graphics` — which is
what keeps the loader tests off the GPU. That's a deliberate constraint on the
test data to preserve a property of the suite.

The path is injected by the build rather than hardcoded or computed from the
working directory:

```cmake
target_compile_definitions(SpaceInvadersTests PRIVATE
    SPACEINVADERS_TEST_FIXTURES="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)
```

`crlf_level` exists for a specific bug: `std::getline` leaves a `\r` on every line
of a Windows-checked-out file, and that `\r` used to reach `std::stof`, which
throws `std::invalid_argument`.

### Testing the data, not just the code

The most valuable file in the suite is `tests/test_level_data.cpp`, and it tests
something unusual: **that the code and the data file agree.**

```cpp
TEST_CASE("every opening tag in world/level1 is closed by its declared tag")
{
    for (const auto& pair : tagPairs)
    {
        size_t linesWithOpenTag = 0, linesAlsoClosed = 0;
        for (const std::string& line : readLevelFile())
        {
            if (line.find(pair.first) == std::string::npos) { continue; }
            linesWithOpenTag++;
            if (line.find(pair.second) != std::string::npos) { linesAlsoClosed++; }
        }
        INFO("tag pair: " << pair.first << " ... " << pair.second);
        CHECK(linesWithOpenTag == linesAlsoClosed);
    }
}
```

A file format has two implementations — the writer and the reader — and the
compiler checks neither against the other. Both halves had drifted:
`ObjectTags` declared `[-END COMPONENT]` while the data used `[-COMPONENT]`, which
meant no component was ever parsed and every game object was built empty.

A companion case asserts that every component name in the data is one the factory
can actually construct. Together they turn "the format is whatever the code
happens to do" into something checkable.

> Any time you have a serialised format, config schema, or protocol, there is a
> test of this shape available, and it is usually the highest-value test you can
> write.

### Testing the error paths

Roughly a third of the suite asserts that things *fail correctly*, because the
original code's failure modes were the dangerous part:

```cpp
TEST_CASE("a missing level file is reported, not ignored")
{
    CHECK_THROWS_AS(
        loader.loadGameObjectsForPlayMode(fixture("does_not_exist"), objects),
        std::runtime_error
    );
}

TEST_CASE("asking a component-less object for a component throws")
{
    GameObject bare;
    bare.setTag("bare");
    CHECK_THROWS_AS(bare.getTransformComponent(), std::runtime_error);
    CHECK_THROWS_AS(bare.getGraphicsComponent(), std::runtime_error);
    CHECK_THROWS_AS(bare.getFirstUpdateComponent(), std::runtime_error);
}
```

That second one pins a real crash. Those accessors index `m_Components` with a
cached position that stays `-1` until the component is added, and
`vector::operator[]` takes an unsigned type — so `-1` becomes `SIZE_MAX` and reads
16 bytes before the array. AddressSanitizer caught it happening on the first
object of the first level.

### The discipline that actually matters

A test that has never failed has never been tested. Every case here was verified
by **reintroducing the bug** and confirming the suite goes red:

```
setLocationX/Y writing m_Height        -> 5 test cases fail
ObjectTags "[-END COMPONENT]"          -> 6 test cases fail
extractor assuming a column-0 tag      -> 1 test case fails
unchecked ifstream on a missing file   -> 1 test case fails
```

This caught two false alarms, both of which turned out to be stale incremental
builds rather than real failures — Make's one-second timestamp granularity losing
a race with a rapid edit-build loop. Worth knowing: if a C++ test result surprises
you right after a revert, force a rebuild before you believe it.

For pre-existing bugs the order is necessarily test-*after*: the code already
exists, so you write the failing test, watch it fail, then fix. True test-first
applies to new work. Both give you the same guarantee — a test observed in both
states.

### Mutation testing: checking the tests

A test that has never failed has never been tested. Every case here was verified
by reintroducing the bug and confirming the suite goes red — and that found three
tests which passed *for the wrong reason*:

- The double-kill test spawned one bullet, so it never exercised the `break` it
  claimed to guard.
- The parked-bullet test used a never-spawned bullet, which the ownership check
  rejects on its own, so `m_IsSpawned` was never the deciding factor.
- Nothing covered the ownership half at all — an invader's bullet hitting an
  invader.

Roughly a 30% failure rate against tests I had just written and believed in. The
full account is [primer 09 §14](09-bug-catalogue.md).

One practical trap: **incremental builds gave a false "passes" three times.**
Make's one-second timestamp granularity loses races with a fast edit-build loop.
Confirm mutation results with a clean rebuild.

### What to write next

1. **`WorldState` transitions.** Wave advance, life loss, game over. Pure integer
   logic, blocked only by it being global mutable state — which makes tests
   order-dependent and is itself the argument for threading it through explicitly.
2. **Invader speed escalation.** `dropDownAndReverse` adjusts `m_Speed` with a
   formula whose operator precedence is ambiguous. A test would pin the intent.
3. **The bullet pool wrapping** when all 14 bullets are in flight at once.
4. **Anything at all covering input handling.** It has zero coverage, has already
   hidden two unreachable-branch bugs, and is the layer an
   [SFML 3 migration](12-sfml-2-to-3-migration.md) would rewrite.

## Related

- [09 — The bug catalogue](09-bug-catalogue.md) — what each test pins down
- [01 — Architecture overview](01-architecture-overview.md) — the seams that made testing possible
