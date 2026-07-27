#pragma once

// What the game actually needs from audio: four sounds it can ask for.
//
// SoundEngine used to be reached through `static` member functions backed by a
// raw `SoundEngine* m_s_Instance` set in the constructor. That has three costs:
// the lifetime is unmanaged, the initialisation order is a hazard, and -- the
// one that mattered here -- there is no way to substitute a silent
// implementation, so any test touching collision logic made noise. Depending on
// this interface instead means callers can be handed a real engine, a silent
// one, or a recording one.
class SoundPlayer
{
    public:
        virtual ~SoundPlayer() = default;

        virtual void playShoot() = 0;
        virtual void playPlayerExplode() = 0;
        virtual void playInvaderExplode() = 0;
        virtual void playClick() = 0;
};
