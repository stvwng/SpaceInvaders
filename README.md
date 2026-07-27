# SpaceInvaders

A Space Invaders clone in C++ and SFML — originally written as a C++ learning
project, now debugged, tested, and documented.

Roughly 3,400 lines across 40 classes, built on a component architecture: there
is no `Invader` class, only a `GameObject` that becomes an invader by holding a
transform, a sprite, and an update component. What each object *is* comes from a
text file in `world/`, not from a type hierarchy.

## Quick start

```bash
brew install cmake sfml@2
cmake -B build
cmake --build build
cd build && ./SpaceInvaders
```

The game must be launched from the build directory — every asset path in the
source is relative to the working directory, and CMake copies `graphics/`,
`sound/`, `fonts/`, and `world/` next to the binary.

### Controls

| Key | Action |
|---|---|
| ← → ↑ ↓ | Move the ship |
| Space | Fire |
| Esc | Back to the menu (quits from the menu) |

A connected gamepad moves the ship via the left stick. Gamepad *firing* is
currently broken — see below.

## Requirements

- **CMake** 3.20+
- **SFML 2.6** — `sfml@2` in Homebrew. Not SFML 3; see below.
- A **C++17** compiler. Developed against Apple Clang on arm64.

SFML 3.0 is a hard API break from the 2.x this code targets: `pollEvent` returns
`std::optional`, keyboard enums became scoped, `FloatRect` swapped
`left`/`top`/`width`/`height` for `position`/`size`, and `Sprite`/`Text` now
require their resource at construction. Pinning `sfml@2` keeps "make it work"
separate from "make it modern." Migration is planned.

> If you tried this on Apple Silicon years ago and hit a wall — that's fixed
> upstream. Homebrew now ships native `arm64` bottles for SFML. No VM needed.

## Build options

| Flag | Default | Effect |
|---|---|---|
| `-DSPACEINVADERS_FULLSCREEN=ON` | off | Launch fullscreen instead of windowed |
| `-DSPACEINVADERS_SANITIZE=ON` | off | AddressSanitizer + UndefinedBehaviorSanitizer |
| `-DSPACEINVADERS_DEBUG_LOG=ON` | off | Compile the diagnostic logging blocks |

Windowed is the default deliberately: a crash while fullscreen on macOS can leave
you with no visible way back to the desktop.

## Tests

```bash
cmake --build build --target SpaceInvadersTests
ctest --test-dir build          # or: cd build && ./SpaceInvadersTests
```

19 cases, 435 assertions. The suite is silent and windowless by construction — no
test opens a `RenderWindow`, loads a texture, or plays a sound — so it runs
anywhere without stealing focus.

Game code builds as a static library (`spaceinvaders_lib`) that both the
executable and the test binary link, so tests exercise the real objects.

## Layout

```
*.cpp, *.h          game source (flat, one class per pair)
world/level1        level data: objects, positions, component lists
graphics/ sound/ fonts/
tests/              doctest suite + level fixtures
third_party/        vendored doctest, so the build works offline
docs/               primers — see below
```

## Documentation

[`docs/`](docs/) contains primers written for re-reading this codebase cold. Each
one goes overview → ELI5 → engineer-level detail.

- [**Architecture overview**](docs/01-architecture-overview.md) — the four
  layers, the game loop, the component model, and the two places this design
  gets dependency inversion genuinely right.
- [**The bug catalogue**](docs/09-bug-catalogue.md) — thirteen defect classes
  found in this project, each tied to the C++ rule it turns on. Start here.
- [**Testing C++ with doctest**](docs/10-testing.md) — how the suite is
  structured, and why some code isn't testable yet.

## State of the project

This code had **never been compiled** — the first header in the dependency graph
contained `#include <GameObjectSharer.h"`, with mismatched delimiters. Getting it
running surfaced two layers of defects: names that had drifted because nothing
ever checked them, and logic that compiled fine and was simply wrong.

Currently:

- Clean build at `-Wall -Wextra -Wpedantic`, zero warnings
- Clean under AddressSanitizer and UndefinedBehaviorSanitizer
- 19/19 tests passing
- A headless harness drives 1,800 simulated frames correctly

Known open bug:

- **Gamepad fire does nothing.** `GameInputHandler::handleGamepad` guards the
  shot on `isButtonPressed(0, 1) && mButtonPressed`, but `mButtonPressed` starts
  `false` and is only ever assigned `false` — nothing sets it `true`, so the
  branch is unreachable. It reads like an intended debounce (fire on the press,
  not every frame it's held) with the condition inverted and the release case
  missing. Keyboard fire is unaffected.

Not yet done:

- **Visual confirmation.** No one has watched it render end to end.
- **Architecture rework** — replacing the stringly-typed component tags with
  `enum class`, const-correctness, and dependency injection for the two
  raw-static singletons. That last one is what currently blocks testing the
  collision arithmetic.
- **Performance measurement**, and nine more primers.

The most instructive bug, for anyone browsing: `ObjectTags` declared the
component closing tag as `[-END COMPONENT]` while every level file writes
`[-COMPONENT]`. Because the parser sliced by tag *length* instead of searching
for tags, it returned a plausible truncation rather than failing — so every game
object was built with zero components, and the game would have rendered an empty
screen. AddressSanitizer found it as a read 16 bytes before an array.
[Full write-up.](docs/09-bug-catalogue.md)

## Credits

Structure follows John Horton's *Beginning C++ Game Programming*. Graphics,
sound, and font assets are from that book's companion material.
