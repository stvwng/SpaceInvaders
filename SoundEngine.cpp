#include <SFML/Audio.hpp>
#include <assert.h>
#include <stdexcept>
#include <string>
#include "SoundEngine.h"

using namespace std;
using namespace sf;

SoundEngine* SoundEngine::m_s_Instance = nullptr;

namespace
{
    void loadOrThrow(SoundBuffer& buffer, const string& path)
    {
        if (!buffer.loadFromFile(path))
        {
            throw std::runtime_error("SoundEngine - could not load \"" + path + "\"");
        }
    }
}

SoundEngine::SoundEngine()
{
    assert(m_s_Instance == nullptr);
    m_s_Instance = this;

    // These paths were "sound/playerExplode.ogg" and "sound/invaderExplode.ogg"
    // while the files on disk are all lowercase. macOS's default filesystem is
    // case-insensitive so it worked here, but it fails on Linux -- a good
    // candidate for why sound never worked on the Linux VM this project was
    // originally developed on. The loadFromFile results were discarded, so the
    // failure was completely silent either way.
    loadOrThrow(m_ShootBuffer, "sound/shoot.ogg");
    loadOrThrow(m_PlayerExplodeBuffer, "sound/playerexplode.ogg");
    loadOrThrow(m_InvaderExplodeBuffer, "sound/invaderexplode.ogg");
    loadOrThrow(m_ClickBuffer, "sound/click.ogg");

    // Associate sounds with buffers
    m_ShootSound.setBuffer(m_ShootBuffer);
    m_PlayerExplodeSound.setBuffer(m_PlayerExplodeBuffer);
    m_InvaderExplodeSound.setBuffer(m_InvaderExplodeBuffer);
    m_ClickSound.setBuffer(m_ClickBuffer);
}

void SoundEngine::playShoot()
{
    m_s_Instance->m_ShootSound.play();
}

void SoundEngine::playPlayerExplode()
{
    m_s_Instance->m_PlayerExplodeSound.play();
}

void SoundEngine::playInvaderExplode()
{
    m_s_Instance->m_InvaderExplodeSound.play();
}

void SoundEngine::playClick()
{
    m_s_Instance->m_ClickSound.play();
}
