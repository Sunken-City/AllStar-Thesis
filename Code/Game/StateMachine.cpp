#include "Game/StateMachine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

static GameState m_state = STARTUP;
Event<unsigned int> OnStateSwitch;

bool SetGameState(GameState newState)
{
    if (m_state != newState)
    {
        DebuggerPrintf("Changed State from %s to %s\n", GetStateString(m_state), GetStateString(newState));
        m_state = newState;
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

GameState GetGameState()
{
    return m_state;
}

const char* GetStateString(GameState state)
{
    switch (state)
    {
    case STARTUP:
        return "Startup";
    case MAIN_MENU:
        return  "Main Menu";
    case PLAYING:
        return "Playing";
    case PAUSED:
        return "Paused";
    case GAME_OVER:
        return "Shutdown";
    case SHUTDOWN:
        return "Shutdown";
    default:
        ERROR_RECOVERABLE("State Machine was put into an undocumented state. (Have you updated GetStateString?)");
        return "";
    }
}
