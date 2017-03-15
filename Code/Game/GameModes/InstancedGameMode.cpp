#include "Game/GameModes/InstancedGameMode.hpp"
#include "../TheGame.hpp"
#include "../Entities/PlayerShip.hpp"
#include "../Entities/Projectiles/Projectile.hpp"

//-----------------------------------------------------------------------------------
InstancedGameMode::InstancedGameMode()
    : GameMode()
{
    m_gameLengthSeconds = 121.0f;
}

//-----------------------------------------------------------------------------------
InstancedGameMode::~InstancedGameMode()
{
    for (GameMode* mode : m_gameModeInstances)
    {
        delete mode;
    }
    m_gameModeInstances.clear();
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
    m_isPlaying = true;
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
void InstancedGameMode::ShowBackground()
{
    for (GameMode* instance : m_gameModeInstances)
    {
        instance->ShowBackground();
    }
}

//-----------------------------------------------------------------------------------
void InstancedGameMode::SpawnBullet(Projectile* bullet)
{
    bullet->m_currentGameMode = bullet->m_owner->m_currentGameMode;
    for (GameMode* instance : m_gameModeInstances)
    {
        if (instance == bullet->m_owner->m_currentGameMode)
        {
            instance->m_newEntities.push_back(bullet);
        }
    }
}

//-----------------------------------------------------------------------------------
void InstancedGameMode::HideBackground()
{
    for (GameMode* instance : m_gameModeInstances)
    {
        instance->HideBackground();
    }
}

//-----------------------------------------------------------------------------------
void InstancedGameMode::AddGameModeInstance(GameMode* mode, PlayerShip* player)
{
    m_gameModeInstances.push_back(mode);
    m_playerGamemodeMap[mode] = player;
}
