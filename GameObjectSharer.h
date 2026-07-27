#pragma once
#include <vector>
#include <string>

class GameObject;

class GameObjectSharer {
    public:
        virtual ~GameObjectSharer() = default;

        virtual std::vector<GameObject>& getGameObjectsWithGOS() = 0;
        virtual GameObject& findFirstObjectWithTag(std::string tag) = 0;
};