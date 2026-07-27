#pragma once
#ifndef SOUND_ENGINE_H
#define SOUND_ENGINE_H

#include <SFML/Audio.hpp>
#include "SoundPlayer.h"

// Loads the game's sounds and plays them. Owned by GameEngine and handed down
// as a SoundPlayer& to everything that needs it.
//
// This used to expose `static` play* functions backed by a raw
// `static SoundEngine* m_s_Instance` assigned in the constructor, so every call
// site reached it globally and nothing could substitute a silent implementation
// for tests. It also carried m_UhSound and m_OhSound, which had no buffers and
// were never played.
class SoundEngine : public SoundPlayer
{
    private:
        sf::SoundBuffer m_ShootBuffer;
        sf::SoundBuffer m_PlayerExplodeBuffer;
        sf::SoundBuffer m_InvaderExplodeBuffer;
        sf::SoundBuffer m_ClickBuffer;

        sf::Sound m_ShootSound;
        sf::Sound m_PlayerExplodeSound;
        sf::Sound m_InvaderExplodeSound;
        sf::Sound m_ClickSound;

    public:
        // Throws std::runtime_error if any sound file cannot be loaded.
        SoundEngine();

        // From SoundPlayer
        void playShoot() override;
        void playPlayerExplode() override;
        void playInvaderExplode() override;
        void playClick() override;
};
#endif
