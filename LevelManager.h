#pragma once
#include "GameObjectSharer.h"
#include <vector>
#include <string>
#include <iostream>
#include "GameObject.h"

using namespace std;

class LevelManager : public GameObjectSharer
{
    private:
        vector<GameObject> m_GameObjects;

        const std::string WORLD_FOLDER = "world";
        const std::string SLASH = "/";

        void runStartPhase();
        void activateAllGameObjects();

    public:
        vector<GameObject>& getGameObjects();
        void loadGameObjectsForPlayMode(string screenToLoad);

        // From GameObjectSharer interface
        vector<GameObject>& getGameObjectsWithGOS()
        {
            return m_GameObjects;
        }

        GameObject& findFirstObjectWithTag(string tag)
        {
            auto it = m_GameObjects.begin();
            auto end = m_GameObjects.end();
            for (; it != end; ++it)
            {
                if ((*it).getTag() == tag)
                {
                    return (*it);
                }
            }

            #ifdef SPACEINVADERS_DEBUG_LOG
            std::cout << "LevelManager.h findFirstObjectWithTag() - TAG NOT FOUND ERROR: "
                      << tag << std::endl;
            #endif

            return m_GameObjects[0];
        }
};