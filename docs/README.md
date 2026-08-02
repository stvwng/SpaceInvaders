# SpaceInvaders — primers

A set of documents for re-reading this codebase cold. Each one is structured the
same way:

1. **Overview** — what this piece does and why it exists.
2. **ELI5** — the analogy, no jargon.
3. **For a SWE** — the real mechanics, with before/after code from *this* repo
   and the C++ rules involved.

You wrote this game as a C++ learning exercise around 2023, following the shape
of Horton's *Beginning C++ Game Programming*. It had never been compiled. These
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

### Tests and benchmark

```bash
ctest --test-dir build                    # 32 cases, 481 assertions
cd build && ./SpaceInvadersBench          # simulation timings
```

The suite is deliberately silent and windowless — no test constructs a
`SoundEngine`, opens a `RenderWindow`, or loads a texture — so it runs anywhere
without making noise or stealing focus.

> **Note on incremental builds.** If a test result surprises you immediately
> after reverting a change, force a rebuild (`touch` the file, or delete
> `build/`). Make's one-second timestamp granularity can lose a race with a fast
> edit-build loop. This produced three false results while writing these tests.

## Why SFML 2, not 3

SFML 3.0 is a hard API break: `pollEvent` returns `std::optional`, keyboard enums
became scoped, `FloatRect` swapped `left`/`top`/`width`/`height` for
`position`/`size`, and `Sprite`/`Text` now require their resource at
construction. Pinning `sfml@2` kept "make it work" separate from "make it
modern" — so a failure was unambiguously this project's bug rather than an API
change. Full migration inventory in [primer 12](12-sfml-2-to-3-migration.md).

The Apple Silicon problem that pushed this to a Linux VM originally is gone:
Homebrew ships native `arm64` bottles for both `sfml@2` and `sfml`.

## The primers

| # | Primer | C++ concepts |
|---|---|---|
| [01](01-architecture-overview.md) | **Architecture overview** | translation units, linking, headers vs sources |
| [02](02-game-loop.md) | The game loop | delta time, fixed vs variable timestep, tunnelling |
| [03](03-screens-and-dependency-inversion.md) | Screens & dependency inversion | pure virtual, abstract classes, breaking dependency cycles |
| [04](04-component-model.md) | The component model | composition vs inheritance, vtables, `static_` vs `dynamic_pointer_cast` |
| [05](05-smart-pointers-and-ownership.md) | Smart pointers & ownership | `unique_ptr`, `shared_ptr`, `move`, type erasure, virtual destructors |
| [06](06-level-loading.md) | Level loading | file I/O, the factory pattern, parsing untrusted data |
| [07](07-collision-detection.md) | Collision detection | AABB intersection, state machines, pooled objects |
| [08](08-resource-management.md) | Resource management | singletons vs DI, static init order, RAII, member init order |
| [09](09-bug-catalogue.md) | **The bug catalogue** | UB, integer division, uninitialised members, dangling references |
| [10](10-testing.md) | Testing with doctest | seams, test doubles, mutation testing |
| [11](11-performance.md) | Performance | measuring first, atomic refcounts, complexity vs constants |
| [12](12-sfml-2-to-3-migration.md) | SFML 2 → 3 migration | API evolution, `std::optional`, scoped enums |

## Start here

If you read one, read **[the bug catalogue](09-bug-catalogue.md)**. It is grounded
in fifteen classes of defect that were actually in this repo, which teaches the
underlying C++ rules far better than a generic list of undefined behaviour.

If you read two, add **[performance](11-performance.md)** — not for the numbers,
but because it is a worked example of measuring before optimising, and of the
measurement overturning the plan.

## Current state

- Clean build at `-Wall -Wextra -Wpedantic`, zero warnings
- Clean under AddressSanitizer and UndefinedBehaviorSanitizer
- 32 tests, 481 assertions
- Simulation costs 0.078% of a 60fps frame

**Not yet done:** nobody has watched the game render end to end. Everything above
is verified by compiler, sanitizers, tests and a benchmark — but rendering, input
handling and screen transitions have no automated coverage by construction, and
that is where the remaining risk lives.
