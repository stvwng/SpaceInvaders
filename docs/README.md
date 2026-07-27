# SpaceInvaders — primers

A set of documents for re-reading this codebase cold. Each one is structured the
same way:

1. **Overview** — what this piece does and why it exists.
2. **ELI5** — the analogy, no jargon.
3. **For a SWE** — the real mechanics, with before/after code from *this* repo
   and the C++ rules involved.

You wrote this game as a C++ learning exercise around 2023, following the shape
of Horton's *Beginning C++ Game Programming*. It was never compiled. These
primers cover both what you built and the language concepts it leans on.

## Build and run

```bash
brew install cmake sfml@2          # SFML 2.6.2, native arm64
cmake -B build
cmake --build build
cd build && ./SpaceInvaders
```

Assets are copied next to the binary at build time, so the game must be launched
from the build directory (every asset path in the source is relative to the
working directory).

### Options

| Flag | Default | Effect |
|---|---|---|
| `-DSPACEINVADERS_FULLSCREEN=ON` | off | Launch fullscreen instead of windowed |
| `-DSPACEINVADERS_SANITIZE=ON` | off | AddressSanitizer + UndefinedBehaviorSanitizer |
| `-DSPACEINVADERS_DEBUG_LOG=ON` | off | Compile the diagnostic logging blocks |

### Tests

```bash
cmake --build build --target SpaceInvadersTests
cd build && ./SpaceInvadersTests      # or: ctest --test-dir build
```

The suite is deliberately silent and windowless — no test constructs a
`SoundEngine`, opens a `RenderWindow`, or loads a texture — so it runs anywhere
without making noise or stealing focus.

> **Note on incremental builds.** If a test fails immediately after you revert a
> change, force a rebuild (`touch` the file, or delete `build/`). Make's
> one-second timestamp granularity can miss a file rewritten in the same second
> as the previous build.

## Why SFML 2, not 3

SFML 3.0 is a hard API break: `pollEvent` returns `std::optional`, keyboard enums
became scoped, `FloatRect` swapped `left`/`top`/`width`/`height` for
`position`/`size`, and `Sprite`/`Text` now require their resource at
construction. This code is SFML 2.x throughout. Pinning `sfml@2` kept "make it
work" separate from "make it modern" — you can tell your own bugs from an API
change. The migration is its own primer.

The Apple Silicon problem that pushed this to a Linux VM originally is gone:
Homebrew ships native `arm64` bottles for both `sfml@2` and `sfml`.

## The primers

| # | Primer | C++ concepts |
|---|---|---|
| [01](01-architecture-overview.md) | Architecture overview | translation units, linking, headers vs sources |
| [09](09-bug-catalogue.md) | The bug catalogue | UB, integer division, uninitialised members, dangling references |
| [10](10-testing.md) | Testing C++ with doctest | seams, test doubles, making logic testable |

Still to be written: the game loop, screens and dependency inversion, the
component model, smart pointers and ownership, level loading, collision
detection, resource management, performance, and the SFML 2 → 3 migration.

## Start here

If you read one, read **[the bug catalogue](09-bug-catalogue.md)**. It is grounded
in defects that were actually in this repo, which teaches the underlying C++
rules far better than a generic list of undefined behaviour.
