#include "Game/GameModes/InstancedGameMode.hpp"
#include "../TheGame.hpp"
#include "../Entities/PlayerShip.hpp"

//-----------------------------------------------------------------------------------
InstancedGameMode::InstancedGameMode()
    : GameMode()
{
    m_gameLengthSeconds = 121.0f;
}

//-----------------------------------------------------------------------------------
InstancedGameMode::~InstancedGameMode()
{

}

//-----------------------------------------------------------------------------------
void InstancedGameMode::Initialize(const std::vector<PlayerShip*>& players)
{
    GameMode::Initialize(players);
    for (GameMode* mode : m_gameModeInstances)
    {
        std::vector<PlayerShip*> playerSubset;
        playerSubset.push_back(m_playerGamemodeMap[mode]);
        mode->Initialize(playerSubset);
    }
}

//-----------------------------------------------------------------------------------
void InstancedGameMode::CleanUp()
{
    for (GameMode* instance : m_gameModeInstances)
    {
        instance->CleanUp();
    }
}

//-----------------------------------------------------------------------------------
void InstancedGameMode::Update(float deltaSeconds)
{
    for (GameMode* instance : m_gameModeInstances)
    {
        instance->Update(deltaSeconds);
    }
}

//-----------------------------------------------------------------------------------
void InstancedGameMode::AddGameModeInstance(GameMode* mode, PlayerShip* player)
{
    m_gameModeInstances.push_back(mode);
    m_playerGamemodeMap[mode] = player;
}
