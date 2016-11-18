#pragma once
#include "Engine/Core/Event.hpp"

enum GameState
{
    STARTUP = 0,
    MAIN_MENU,
    ASSEMBLY_PLAYING,
    ASSEMBLY_RESULTS,
    MINIGAME_PLAYING,
    MINIGAME_RESULTS,
    GAME_RESULTS_SCREEN,
    SHUTDOWN,
    NUM_STATES
};

bool SetGameState(GameState state);
GameState GetGameState();
const char* GetStateString(GameState state);

//ONE SHOT FUNCTIONS ONLY. These get registered when switching to the state, and then are fired when swapping away from that state and removed.
extern Event<unsigned int> OnStateSwitch;




