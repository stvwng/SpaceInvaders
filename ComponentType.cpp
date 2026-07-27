#include "ComponentType.h"

ObjectTag toObjectTag(const std::string& name)
{
    // The level file's exact spellings. Note "Player" is capitalised while
    // "invader" and "bullet" are not -- an inconsistency that used to be
    // load-bearing, because these strings were compared directly all over the
    // codebase. It is now confined to this one function.
    if (name == "Player")  { return ObjectTag::Player; }
    if (name == "invader") { return ObjectTag::Invader; }
    if (name == "bullet")  { return ObjectTag::Bullet; }
    return ObjectTag::Unknown;
}

const char* toString(ObjectTag tag)
{
    switch (tag)
    {
        case ObjectTag::Player:  return "Player";
        case ObjectTag::Invader: return "invader";
        case ObjectTag::Bullet:  return "bullet";
        case ObjectTag::Unknown: return "unknown";
    }
    return "unknown";
}

const char* toString(ComponentType type)
{
    switch (type)
    {
        case ComponentType::Update:    return "update";
        case ComponentType::Graphics:  return "graphics";
        case ComponentType::Transform: return "transform";
        case ComponentType::Collider:  return "collider";
    }
    return "unknown";
}

const char* toString(ComponentSpecificType type)
{
    switch (type)
    {
        case ComponentSpecificType::Player:    return "player";
        case ComponentSpecificType::Invader:   return "invader";
        case ComponentSpecificType::Bullet:    return "bullet";
        case ComponentSpecificType::Rect:      return "rect";
        case ComponentSpecificType::Standard:  return "standard";
        case ComponentSpecificType::Transform: return "transform";
    }
    return "unknown";
}
