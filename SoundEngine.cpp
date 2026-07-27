#include "SoundEngine.h"
#include <stdexcept>
#include <string>

namespace
{
    void loadOrThrow(sf::SoundBuffer& buffer, const std::string& path)
    {
        if (!buffer.loadFromFile(path))
        {
            throw std::runtime_error("SoundEngine - could not load \"" + path + "\"");
        }
    }
}

SoundEngine::SoundEngine()
{
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
    m_ShootSound.play();
}

void SoundEngine::playPlayerExplode()
{
    m_PlayerExplodeSound.play();
}

void SoundEngine::playInvaderExplode()
{
    m_InvaderExplodeSound.play();
}

void SoundEngine::playClick()
{
    m_ClickSound.play();
}
