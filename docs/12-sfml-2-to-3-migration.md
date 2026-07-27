# 12 — Migrating from SFML 2 to SFML 3

## Overview

This game targets SFML 2.6.2, pinned deliberately. SFML 3.0 (released December
2024) is a hard API break — not a deprecation cycle, but a coordinated
modernisation that touches windowing, events, geometry, resource loading, and
even the CMake target names.

Pinning `sfml@2` kept two problems apart: *make it work* and *make it modern*.
When something broke during the debugging phases, it was unambiguously this
project's bug rather than an API change. That separation is worth keeping in
mind as a general tactic — it is much cheaper to debug one variable at a time.

This primer inventories exactly what this codebase would need to change.

## ELI5

Imagine a tool company redesigning its whole range at once.

The old screwdriver had a flat handle and you were expected to know which way to
turn it. The new one has a grip that only fits one way, so you *can't* hold it
wrong. The old tape measure gave you a number and you had to remember whether it
meant inches or centimetres. The new one hands you a thing that knows its own
units.

Every change makes mistakes harder to make. But none of your old tools fit the
new holders, so you have to go around the workshop swapping them out. Nothing
about the *job* changed — only what the tools look like.

That's SFML 3. The library didn't get new powers; it got harder to misuse.

## For a SWE

### The theme

Almost every change follows one principle: **make invalid states unrepresentable.**

- A `Sprite` with no texture was legal and drew nothing. Now a texture is a
  constructor argument.
- `pollEvent` filled a reference and returned `bool`, so you could read an event
  that was never written. Now it returns `std::optional<Event>`.
- `sf::Keyboard::A` sat in the enclosing scope and converted to `int`. Now
  `sf::Keyboard::Key::A` is scoped and does not implicitly convert.
- `setRotation(90)` — degrees or radians? Now `setRotation(sf::degrees(90))`.

This is the same shift this project made internally when component tags became
`enum class` ([primer 04](04-component-model.md)). SFML 3 is that idea applied
across a library.

### What this codebase would have to change

| Area | Occurrences here | SFML 3 form |
|---|---|---|
| CMake targets | 1 line | `sfml-graphics` → `SFML::Graphics` |
| `pollEvent` loop | 1 (`Screen.cpp`) | returns `std::optional<Event>` |
| `event.type` / `event.key` | 14 | `event->is<T>()` / `event->getIf<T>()` |
| `Keyboard::X` | 7 distinct keys | `Keyboard::Key::X` |
| `FloatRect` members | 4 | `.left` → `.position.x`, etc. |
| `intersects` | 9 (incl. tests) | `findIntersection()` → `std::optional` |
| `Sprite`/`Text` default-construct | 7 files | resource required at construction |
| `Font::loadFromFile` | 1 (`FontStore`) | renamed `openFromFile` |
| `sf::Uint32` | 2 | `std::uint32_t` |
| `VideoMode(w, h)` | 5 | `VideoMode({w, h})` |
| `Style::Fullscreen` | 1 | `State::Fullscreen` |

A day's work, mostly mechanical, and the compiler finds nearly all of it.

### The event loop

This is the biggest structural change. Current:

```cpp
Event event;
while (window.pollEvent(event))
{
    for (auto it = m_InputHandlers.begin(); it != m_InputHandlers.end(); ++it)
    {
        (*it)->handleInput(window, event);
    }
}
```

SFML 3:

```cpp
while (const std::optional<sf::Event> event = window.pollEvent())
{
    for (auto it = m_InputHandlers.begin(); it != m_InputHandlers.end(); ++it)
    {
        (*it)->handleInput(window, *event);
    }
}
```

And the dispatch inside `InputHandler` changes shape entirely. Now:

```cpp
if (event.type == Event::KeyPressed)  { handleKeyPressed(event, window); }
if (event.type == Event::KeyReleased) { handleKeyReleased(event, window); }
```

SFML 3 makes `Event` a variant, so you ask for a type and get the payload or
nothing:

```cpp
if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
{
    handleKeyPressed(*keyPressed, window);   // keyPressed->code is the key
}
```

Two real improvements. You cannot read `event.key.code` on a mouse event any
more — in SFML 2 that was a union access, silently returning garbage. And
`event.getIf<T>()` returns a pointer that is null on mismatch, so the check and
the access are one step.

This would ripple through the `InputHandler` hierarchy's signatures — the one
genuinely *design*-level change in the migration, not just a rename.

### Rect: geometry as vectors

```cpp
// SFML 2 — what RectColliderComponent does now
m_Collider.left   = x;
m_Collider.top    = y;
m_Collider.width  = width;
m_Collider.height = height;

// SFML 3
m_Collider.position = {x, y};
m_Collider.size     = {width, height};
```

And overlap:

```cpp
// SFML 2
if (a.intersects(b)) { /* hit */ }

// SFML 3
if (const std::optional<sf::FloatRect> overlap = a.findIntersection(b)) { /* hit */ }
```

`findIntersection` returns the overlapping rectangle, not just a yes/no. This
codebase only needs the boolean, so the `optional` is used purely for its
contextual conversion — but for anything doing resolution rather than detection,
the overlap rectangle is what you actually want.

### Resources: construct-or-throw

SFML 3 adds throwing constructors alongside the existing two-step form:

```cpp
// Both valid in SFML 3
const sf::Texture texture("graphics/invader1.png");   // throws sf::Exception on failure

sf::Texture texture;
if (!texture.loadFromFile("graphics/invader1.png")) { /* handle */ }
```

That fits this project well, since `BitmapStore` and `FontStore` already
load-or-throw. It would simplify to:

```cpp
auto [it, inserted] = loaded.try_emplace(filename, filename);   // throws on failure
```

Watch for `sf::Font::loadFromFile` being **renamed to `openFromFile`** — fonts
are streamed from the file rather than loaded wholesale, and the new name says
so. That one is a silent trap: `loadFromFile` simply will not exist, so it is a
compile error, not a behaviour change.

### Sprite and Text need their resource up front

```cpp
// SFML 2 — every one of the seven sites in this project
sf::Sprite m_Sprite;                 // member, default-constructed
m_Sprite.setTexture(someTexture);    // later

// SFML 3
sf::Sprite m_Sprite{someTexture};    // texture required
```

This has a real consequence for class design here: `StandardGraphicsComponent`,
`Button`, `GameScreen` and the UI panels all hold a default-constructed `Sprite`
or `Text` as a member and configure it later. In SFML 3 those members must either
be initialised in the constructor's member-initialiser list, or become
`std::optional<sf::Sprite>`.

The centralised `BitmapStore` and `FontStore` built in
[primer 08](08-resource-management.md) make this *easier*, not harder — the
texture reference is already available before the sprite needs to exist:

```cpp
// SFML 3
StandardGraphicsComponent::StandardGraphicsComponent(const std::string& bitmapName)
    : m_Sprite(BitmapStore::getBitmap("graphics/" + bitmapName + ".png"))
{ }
```

That also removes the two-phase `initializeGraphics()` call, which is a small win
for the same reason SFML 3 made the change: fewer half-built objects.

Note that `getFont()` / `getTexture()` now return **references** rather than
pointers, which reflects the fact that they can no longer be absent.

### Window creation

```cpp
// SFML 2 — GameEngine.cpp today
const sf::Uint32 style = sf::Style::Fullscreen;
m_Window.create(sf::VideoMode(width, height), "Space Invaders", style);

// SFML 3
m_Window.create(sf::VideoMode({width, height}), "Space Invaders",
                sf::Style::Default, sf::State::Fullscreen);
```

Fullscreen was never really a *style* — it is a window state, orthogonal to
whether the window has a title bar or is resizable. SFML 3 splits them, which
happens to make this project's `SPACEINVADERS_FULLSCREEN` option cleaner: the
style stays constant and only the state changes.

`sf::Uint32` also goes away in favour of `std::uint32_t`. SFML predates
`<cstdint>` being universally available; that has not been true for a long time.

### CMake

```cmake
# SFML 2
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
target_link_libraries(spaceinvaders_lib PUBLIC sfml-graphics sfml-window sfml-system sfml-audio)

# SFML 3
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)
target_link_libraries(spaceinvaders_lib PUBLIC SFML::Graphics SFML::Window SFML::System SFML::Audio)
```

Namespaced targets are modern CMake convention: `SFML::Graphics` is an imported
target carrying its own include directories and transitive dependencies, so
nothing else in the build file needs to know where SFML lives.

Also note SFML 3 builds **static** libraries by default, the opposite of SFML 2.

### Things that would not change

Worth knowing what is stable, so the migration is not over-scoped:

- `sf::Vector2f` arithmetic and `.x` / `.y` — unchanged, and gains `length()`,
  `normalized()`, `dot()`.
- `sf::Clock` / `sf::Time` and `restart()` / `asSeconds()` — the game loop is
  untouched.
- `Sound::play()`, `SoundBuffer::loadFromFile` — the audio calls here are fine.
- `RenderWindow::draw` / `clear` / `display`, and `sf::View` — the rendering
  structure is untouched.
- `Joystick` and `Keyboard` become **namespaces** rather than classes, which is
  source-compatible for the static-style calls this project makes.

### Would it be worth doing?

Arguments for: SFML 2 is in maintenance; `sfml@2` will eventually leave Homebrew;
the API is genuinely better; and this project is a *learning* artifact, where
practising a real migration on code you understand is the point.

Arguments against: nothing here needs a SFML 3 feature, and the event-handling
rewrite touches the input layer — which is [the part of this codebase with no
test coverage at all](10-testing.md), and which has already hidden two
unreachable-branch bugs.

If you do it: **write the input tests first.** That is the only part of the
migration where the compiler will not catch your mistakes for you, and it is
exactly where this project's history says bugs hide.

## Related

- [08 — Resource management](08-resource-management.md) — the stores that make Sprite/Text construction easy
- [10 — Testing](10-testing.md) — what has no coverage
- [README](README.md) — why SFML 2 is pinned
