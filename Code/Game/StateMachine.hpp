#pragma once
#include "Engine/Core/Events/Event.hpp"

enum GameState
{
    STARTUP = 0,
    MAIN_MENU,
    PLAYER_JOIN,
    ASSEMBLY_GET_READY,
    ASSEMBLY_PLAYING,
    ASSEMBLY_RESULTS,
    MINIGAME_GET_READY,
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
extern float g_secondsInState;





