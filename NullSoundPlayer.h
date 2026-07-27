#pragma once
#include "SoundPlayer.h"

// A SoundPlayer that does nothing -- the null object pattern.
//
// Lets tests and headless runs drive the full simulation without opening an
// audio device or making a sound, and without any call site needing a
// "if (sound) ..." check.
class NullSoundPlayer : public SoundPlayer
{
    public:
        void playShoot() override {}
        void playPlayerExplode() override {}
        void playInvaderExplode() override {}
        void playClick() override {}
};
