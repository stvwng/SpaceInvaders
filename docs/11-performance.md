# 11 — Performance: measuring, then fixing

## Overview

This primer is as much about *method* as about results. The plan for this project
listed a set of likely optimisations up front. Measurement then said most of them
were not worth doing — and pointed at one that mattered far more than anything on
the list.

The headline: the simulation costs **0.078% of a 60fps frame**. Nothing needed
optimising. But measuring how the cost *grows* revealed a quadratic curve, and
tracing that found a `shared_ptr` returned by value in a hot loop. Fixing it made
collision detection **4.2× faster** and turned quadratic growth into linear.

## ELI5

Suppose you want to speed up your commute.

You *guess* the problem is your slow walk to the station, so you buy expensive
running shoes. But you never timed anything. It turns out the walk is four
minutes and the train is delayed forty. The shoes bought you nothing.

Timing first tells you where the time actually goes. And there's a second trick:
don't just time today's commute — check what happens if the city doubles in size.
Something taking four minutes now but *quadrupling* every time the city grows is a
different problem from something taking four minutes and staying there.

That's exactly what happened here. Today's number said "fine." The growth curve
said "fine *for now*" — and the reason it grew badly turned out to be a single
line doing hidden work.

## For a SWE

### The harness

`bench/benchmark.cpp` builds as `SpaceInvadersBench`. It loads the real
`world/level1` and runs exactly what `GameScreen` runs each frame — every
`GameObject::update`, then `detectCollisions` — with no window and no audio.

```bash
cmake --build build --target SpaceInvadersBench
cd build && ./SpaceInvadersBench
```

Design choices worth copying:

- **Warm-up frames** (200) before recording, so first-touch page faults and cold
  caches do not land in the sample.
- **20,000 samples**, reporting mean, median and p99. A single timing is noise;
  the median resists outliers and p99 shows the tail.
- **Periodic state reset**, so the measured work does not quietly shrink as
  invaders are cleared.
- **Rendering excluded**, and said so loudly — `GameObject::draw` needs a live
  `RenderWindow`. These numbers are simulation only.

### First measurement

```
  GameObject::update x all   mean     3.25 us    0.020% of a 60fps frame
  detectCollisions           mean    34.27 us    0.206%
  simulation frame (both)    mean    37.52 us    0.225%
```

A 60fps frame is 16,667 µs. The simulation used **0.225%** of it.

At that point the correct engineering decision is: **do nothing.** The plan had
listed several optimisations — index ranges, killing `shared_ptr` copies,
`reserve()` on the object vector. Every one of them would have been effort spent
on 0.2% of a budget, and every one would have added risk to code that had just
been made correct.

This is the moment most "performance work" goes wrong. The measurement said stop.

### Second measurement: how it grows

Instead of optimising, the benchmark asks a different question — what happens at
larger scales? It reloads the level N times over:

```
     60 objects (  45 invaders)  ->      32.91 us     0.197% of a frame
    120 objects (  90 invaders)  ->     159.72 us     0.958%
    240 objects ( 180 invaders)  ->     699.30 us     4.196%
    480 objects ( 360 invaders)  ->    2967.04 us    17.802%
    960 objects ( 720 invaders)  ->   11882.75 us    71.295%
```

Double the objects, roughly **quadruple** the time. That is O(n²), and at 960
objects it eats 71% of the frame budget on its own.

This reframes the result. "Fast enough" became "fast enough at exactly this level
size, and catastrophically slow if the level grows" — which is a real fact about
the code, not a hypothetical, since adding rows of invaders is the most obvious
thing anyone would do to this game.

### Finding the cause

The obvious suspect is the loop shape — for every invader, walk the bullets. But
O(invaders × bullets) at 45 × 14 = 630 pairs should not cost 33 µs. That is ~52
nanoseconds per pair for what ought to be four float comparisons.

The real cost was hiding in a helper:

```cpp
for (auto invaderIt = objects.begin(); invaderIt != objects.end(); ++invaderIt)
{
    // ...
    for (; bulletIt != objects.end(); ++bulletIt)
    {
        auto bulletUpdate = static_pointer_cast<BulletUpdateComponent>(
            bulletIt->getFirstUpdateComponent());       // <-- returns shared_ptr BY VALUE

        if (!bulletUpdate->m_IsSpawned || !bulletUpdate->m_BelongsToPlayer) { continue; }
        // ...
    }
}
```

`getFirstUpdateComponent()` returns `shared_ptr<UpdateComponent>` **by value**.
Copying a `shared_ptr` is an **atomic** increment; destroying the copy is an
atomic decrement. Atomics enforce memory ordering across cores, so they are far
more expensive than the arithmetic around them — tens of nanoseconds, not one.

630 pairs × 2 atomic operations = **1,260 atomics per frame**, to answer a
question — *is this bullet in flight and player-owned?* — whose answer is
identical for all 45 invaders.

That also explains the quadratic curve: the per-pair cost was large enough to
dominate, so the pair count drove everything.

### The fix

Answer the question once per frame, not once per pair:

```cpp
m_LiveBullets.clear();                              // member: capacity persists
for (const int bulletIndex : bulletPositions)
{
    GameObject& bullet = objects[static_cast<size_t>(bulletIndex)];
    const BulletUpdateComponent& update = *static_pointer_cast<BulletUpdateComponent>(
        bullet.getFirstUpdateComponent());

    if (update.m_IsSpawned && update.m_BelongsToPlayer) { m_LiveBullets.push_back(&bullet); }
}
if (m_LiveBullets.empty()) { return; }

for (/* each active invader */)
{
    for (GameObject* bulletObject : m_LiveBullets) { /* AABB test only */ }
}
```

Three things changed:

1. The atomic refcount happens **once per bullet**, not once per pair.
2. Typically only one or two of fourteen bullets are in flight, so the inner loop
   is nearly always tiny — and exits immediately when nothing is airborne.
3. `m_LiveBullets` is a member, so its capacity survives between frames and the
   gather never allocates.

### Results

```
  detectCollisions      34.27 us  ->   8.09 us      4.2x
  simulation frame      37.52 us  ->  11.17 us      3.4x
```

And the growth curve:

| Objects | Before | After | Speed-up |
|---:|---:|---:|---:|
| 60 | 32.91 µs | 7.00 µs | 4.7× |
| 120 | 159.72 µs | 13.79 µs | 11.6× |
| 240 | 699.30 µs | 27.40 µs | 25.5× |
| 480 | 2,967.04 µs | 55.40 µs | 53.6× |
| 960 | 11,882.75 µs | 109.87 µs | **108×** |

Quadratic became linear. The speed-up grows with size, which is the signature of
an actual complexity change rather than a constant-factor tweak.

### An honest caveat

**The benchmark flatters this fix.** The simulated player never fires, so live
bullets hover near zero and the early-out fires almost every frame. Real play
would keep one to five of the fourteen airborne. The *shape* of the win holds —
the atomics are hoisted regardless — but the magnitude is optimistic.

Saying so is part of reporting a benchmark. A number without its caveats is a
claim, not a measurement.

### Rechecking correctness after a rewrite

Restructuring the loop meant the 27 passing tests could no longer be assumed to
cover it. Mutation testing — reintroduce a bug, confirm the suite goes red —
found a genuine gap:

```
  actually remove the break -> 1 test fails    ✓ guarded
  drop m_IsSpawned check    -> 1 test fails    ✓ guarded
  drop ownership check      -> 0 tests fail    ✗ NOT guarded
```

Nothing asserted that an *invader's* bullet cannot kill an invader. The existing
"not in flight" test used a de-spawned **player** bullet, so it exercised
`m_IsSpawned` but never the ownership half. Added the missing case; all three
mutations are now caught.

> A test suite that passes after a refactor tells you less than you think. What
> tells you something is a suite that *fails* when you break the thing it claims
> to cover.

### What was on the list and did not get done

| Candidate | Verdict |
|---|---|
| `shared_ptr` by value in `GameObject::draw` | Not measured — `draw` needs a window the benchmark cannot create. Real, unquantified. |
| `UIPanel::getButtons()` returning by value | Startup only. Irrelevant. |
| `reserve()` on the 60-object vector | ~4 reallocations at load. Irrelevant. |
| Font loaded per button | Fixed for [lifetime reasons](08-resource-management.md), not speed. |
| Spatial partitioning for collisions | Unjustified. Linear at 960 objects is 0.66% of a frame. |
| String comparisons in hot loops | Already gone — enums, [primer 04](04-component-model.md). |

Six candidates, one real. That ratio is normal, and it is the argument for
measuring before optimising rather than after.

## Related

- [05 — Smart pointers and ownership](05-smart-pointers-and-ownership.md) — why the atomic is expensive
- [07 — Collision detection](07-collision-detection.md) — what the loop does
- [10 — Testing](10-testing.md) — mutation testing
