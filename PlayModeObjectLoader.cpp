#include "PlayModeObjectLoader.h"
#include "ObjectTags.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

void PlayModeObjectLoader::loadGameObjectsForPlayMode(string pathToFile, vector<GameObject>& gameObjects)
{
    ifstream reader(pathToFile);

    // The stream state used to go unchecked. A missing or unreadable level
    // file then produced zero game objects, and the first thing that happens
    // next is a search for the object tagged "Player" -- which read off the end
    // of the empty vector.
    if (!reader.is_open())
    {
        throw std::runtime_error("PlayModeObjectLoader - could not open level file \"" + pathToFile + "\"");
    }

    string lineFromFile;

    while (getline(reader, lineFromFile))
    {
        if (lineFromFile.find(ObjectTags::START_OF_OBJECT) != string::npos)
        {
            GameObjectBlueprint bp;
            m_BOP.parseNextObjectForBlueprint(reader, bp);
            m_GameObjectFactoryPlayMode.buildGameObject(bp, gameObjects);
        }
    }
}