#pragma once
#include "GameObjectSharer.h"
#include <string>

using namespace std;

class GameObject;

// Base class of every component in every game object
class Component
{
    public:
        // Components are held as shared_ptr<Component>, which type-erases the
        // deleter and so would destroy correctly even without this. The other
        // interfaces here are held in unique_ptr<Base>, which does not -- so
        // every polymorphic base in this codebase declares one, for consistency
        // and to keep the rule easy to remember.
        virtual ~Component() = default;

        virtual string getType() = 0;
        virtual string getSpecificType() = 0;
        virtual void disableComponent() = 0;
        virtual void enableComponent() = 0;
        virtual bool enabled() = 0;
        virtual void start(GameObjectSharer* gos, GameObject* self) = 0;
};