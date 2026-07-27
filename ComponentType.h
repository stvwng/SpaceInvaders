#pragma once
#include <string>

// Component identity used to be two std::strings per component --
// getType() returning "update" / "graphics" / "transform" / "collider", and
// getSpecificType() returning "player" / "invader" / "bullet" / "rect" /
// "standard". Both were returned **by value**, so every lookup allocated, and
// both were compared with operator==, so a typo was a silent runtime miss
// rather than a compile error.
//
// These names never come from the level file -- they are entirely internal --
// so there is no reason for them to be strings at all.
enum class ComponentType
{
    Update,
    Graphics,
    Transform,
    Collider
};

enum class ComponentSpecificType
{
    Player,
    Invader,
    Bullet,
    Rect,
    Standard,
    Transform
};

// Object tags *do* come from the level file's [NAME] block, so there is a
// string boundary at load time. Converting once, in the factory, means the
// per-frame comparisons in the physics engine are integer compares instead of
// string compares -- and that the old casing inconsistency between "Player",
// "invader" and "bullet" can no longer bite.
enum class ObjectTag
{
    Player,
    Invader,
    Bullet,
    Unknown
};

// Returns ObjectTag::Unknown for any name the game does not recognise. The
// original spelling is kept on the GameObject for diagnostics.
ObjectTag toObjectTag(const std::string& name);

// For error messages and logging.
const char* toString(ObjectTag tag);
const char* toString(ComponentType type);
const char* toString(ComponentSpecificType type);
