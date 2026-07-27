#pragma once
#include "GameObjectSharer.h"
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include "GameObject.h"


class LevelManager : public GameObjectSharer
{
    private:
        std::vector<GameObject> m_GameObjects;

        const std::string WORLD_FOLDER = "world";
        const std::string SLASH = "/";

        void runStartPhase();
        void activateAllGameObjects();

    public:
        std::vector<GameObject>& getGameObjects();
        void loadGameObjectsForPlayMode(std::string screenToLoad);

        // From GameObjectSharer interface
        std::vector<GameObject>& getGameObjectsWithGOS() override
        {
            return m_GameObjects;
        }

        GameObject& findFirstObjectWithTag(std::string tag) override
        {
            auto it = m_GameObjects.begin();
            auto end = m_GameObjects.end();
            for (; it != end; ++it)
            {
                if ((*it).getTagName() == tag)
                {
                    return (*it);
                }
            }

            // This used to return m_GameObjects[0], which is undefined
            // behaviour when the vector is empty -- exactly what happens when
            // the level file fails to open. Callers keep the result as a
            // long-lived reference or raw pointer, so quietly handing back the
            // wrong object corrupts game state far from the real fault. A
            // level with no Player is unrecoverable; fail where the mistake is.
            throw std::runtime_error(
                "LevelManager::findFirstObjectWithTag - no GameObject tagged \"" + tag + "\""
            );
        }
};