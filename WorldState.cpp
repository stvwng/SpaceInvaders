#include "WorldState.h"


// Definitions for WorldState's static data members.
//
// These used to be scattered across four unrelated translation units --
// WORLD_HEIGHT / NUM_INVADERS / NUM_INVADERS_AT_START in GameScreen.cpp,
// WAVE_NUMBER in SelectInputHandler.cpp, and SCORE / LIVES in GameUIPanel.cpp.
// That links, but it means a UI file owns the storage for gameplay state, and
// deleting an apparently unrelated file breaks the link in a way that points
// nowhere useful. One home per class.
//
// Zero-initialised because they have static storage duration; the game assigns
// real starting values in GameScreen::initialize().

int WorldState::WORLD_HEIGHT = 0;
int WorldState::SCORE = 0;
int WorldState::LIVES = 0;
int WorldState::NUM_INVADERS_AT_START = 0;
int WorldState::NUM_INVADERS = 0;
int WorldState::WAVE_NUMBER = 0;
