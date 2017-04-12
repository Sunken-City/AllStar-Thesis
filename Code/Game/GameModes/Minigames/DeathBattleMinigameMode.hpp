#pragma once
#include "Game/GameModes/BaseMinigameMode.hpp"

//-----------------------------------------------------------------------------------
struct DeathBattleStats : public DefaultPlayerStats
{
    DeathBattleStats(PlayerShip* player) : DefaultPlayerStats(player) {};
    virtual ~DeathBattleStats() {};

    float m_timeAlive = -1.0f;
};

class DeathBattleMinigameMode : public BaseMinigameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    DeathBattleMinigameMode();
    virtual ~DeathBattleMinigameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize(const std::vector<PlayerShip*>& players);
    virtual void CleanUp();
    virtual void Update(float deltaSeconds);
    virtual void RankPlayers() override;
    virtual Encounter* GetRandomMinorEncounter(const Vector2& center, float radius) override;
    void SetUpPlayerSpawnPoints();
    void SpawnPlayers();
    void SpawnGeometry();
    void UpdatePlayerScoreDisplay(PlayerShip* player);

    virtual void InitializePlayerData() override;
    virtual void RecordPlayerDeath(PlayerShip* ship) override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};