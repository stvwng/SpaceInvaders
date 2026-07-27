#pragma once
#include <vector>
#include <string>
#include "GameObject.h"
#include "BlueprintObjectParser.h"
#include "GameObjectFactoryPlayMode.h"


class PlayModeObjectLoader
{
    private:
        BlueprintObjectParser m_BOP;
        GameObjectFactoryPlayMode m_GameObjectFactoryPlayMode;

    public:
        void loadGameObjectsForPlayMode(
            std::string pathToFile,
            std::vector<GameObject>& gameObjects
        );
};