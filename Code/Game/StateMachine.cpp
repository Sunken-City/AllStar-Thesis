#include "Game/StateMachine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

static GameState g_state = STARTUP;
float g_secondsInState = 0.0f;
bool g_isTransitioningStates = false;
Event<unsigned int> OnStateSwitch;

//-----------------------------------------------------------------------------------
bool SetGameState(GameState newState)
{
    if (g_state != newState)
    {
        DebuggerPrintf("Changed State from %s to %s\n", GetStateString(g_state), GetStateString(newState));
        EndTransitioning();
        g_state = newState;
        g_secondsInState = 0;
        OnStateSwitch.Trigger((unsigned int)newState);
        OnStateSwitch.UnregisterAllSubscriptions();
        return true;
    }
    else
    {
        ERROR_RECOVERABLE("State machine was set to the same state.");
        return false;
    }
}

//-----------------------------------------------------------------------------------
GameState GetGameState()
{
    return g_state;
}

//-----------------------------------------------------------------------------------
const char* GetStateString(GameState state)
{
    switch (state)
    {
    case STARTUP:
        return "Startup";
    case MAIN_MENU:
        return  "Main Menu";
    case MAIN_MENU_TRANSITION:
        return  "Main Menu Transition";
    case PLAYER_JOIN:
        return "Player Join";
    case ASSEMBLY_GET_READY:
        return "Assembly Get Ready";
    case ASSEMBLY_PLAYING:
        return "Assembly Playing";
    case ASSEMBLY_RESULTS:
        return "Assembly Results";
    case GAME_RESULTS_SCREEN:
        return "Game Results";
    case SHUTDOWN:
        return "Shutdown";
    case MINIGAME_GET_READY:
        return "Minigame Get Ready";
    case MINIGAME_PLAYING:
        return "Minigame Playing";
    case MINIGAME_RESULTS:
        return "Minigame Results";
    default:
        ERROR_RECOVERABLE("State Machine was put into an undocumented state. (Have you updated GetStateString?)");
        return "";
    }
}

//-----------------------------------------------------------------------------------
bool IsTransitioningStates()
{
    return g_isTransitioningStates;
}

//-----------------------------------------------------------------------------------
void BeginTransitioning()
{
    g_isTransitioningStates = true;
}

//-----------------------------------------------------------------------------------
void EndTransitioning()
{
    g_isTransitioningStates = false;
}

