#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include "GameScreen.h"
#include "ScreenManagerRemoteControl.h"
#include "SelectScreen.h"
#include "LevelManager.h"
#include <iostream>


class ScreenManager : public ScreenManagerRemoteControl
{
    private:
        std::map <std::string, std::unique_ptr<Screen>> m_Screens;
        LevelManager m_LevelManager;

        // Owned by GameEngine, which outlives this.
        SoundPlayer& m_SoundPlayer;

    protected:
        std::string m_CurrentScreen = "Select";

    public:
        ScreenManager(sf::Vector2i res, SoundPlayer& soundPlayer);
        void update(float dt);
        void draw(sf::RenderWindow& window);
        void handleInput(sf::RenderWindow& window);

        // From ScreenManagerRemoteControl interface
        // see https://stackoverflow.com/questions/48255775/c-qualified-name-is-not-allowed-in-member-declaration
        // for why ScreenManagerRemoteControl namespace is not used
        void switchScreens(std::string screenToSwitchTo) override
        {
            m_CurrentScreen = "" + screenToSwitchTo;
            m_Screens[m_CurrentScreen]->initialize();
        }

        void loadLevelInPlayMode(std::string screenToLoad) override
        {
            m_LevelManager.getGameObjects().clear();
            m_LevelManager.loadGameObjectsForPlayMode(screenToLoad);
            switchScreens("Game");
        }

        std::vector<GameObject>& getGameObjects() override
        {
            return m_LevelManager.getGameObjects();
        }

        GameObjectSharer& shareGameObjectSharer() override
        {
            return m_LevelManager;
        }

        SoundPlayer& shareSoundPlayer() override
        {
            return m_SoundPlayer;
        }

};