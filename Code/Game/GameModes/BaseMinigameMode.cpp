#include "Game/GameModes/BaseMinigameMode.hpp"
#include "../TheGame.hpp"
#include "../Entities/PlayerShip.hpp"

//-----------------------------------------------------------------------------------
BaseMinigameMode::BaseMinigameMode()
    : GameMode()
{
    m_gameLengthSeconds = 121.0f;
}

//-----------------------------------------------------------------------------------
BaseMinigameMode::~BaseMinigameMode()
{

}

//-----------------------------------------------------------------------------------
void BaseMinigameMode::EndGameIfTooFewPlayers()
{
    int numPlayersAlive = 0;
    for (PlayerShip* player : TheGame::instance->m_players)
    {
        numPlayersAlive += player->IsAlive() ? 1 : 0;
    }
    if ((numPlayersAlive < 2) && (GetTimerSecondsElapsed() < m_gameLengthSeconds))
    {
        SetTimeRemaining(0.0f);
    }
}