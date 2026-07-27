#pragma once
#include <string>
#include <vector>
#include "GameObject.h"
#include "GameObjectSharer.h"
#include "SoundPlayer.h"


class ScreenManagerRemoteControl
{
    public:
        virtual ~ScreenManagerRemoteControl() = default;

        virtual void switchScreens(std::string screenToSwitchTo) = 0;
        virtual void loadLevelInPlayMode(std::string screenToLoad) = 0;
        virtual std::vector<GameObject>& getGameObjects() = 0;
        virtual GameObjectSharer& shareGameObjectSharer() = 0;

        // Screens and input handlers already hold this interface, so it is the
        // natural place to hand down audio too -- mirroring
        // shareGameObjectSharer() rather than inventing a second wiring path.
        virtual SoundPlayer& shareSoundPlayer() = 0;
};