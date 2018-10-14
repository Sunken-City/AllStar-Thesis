#pragma once
#include "Game/GameModes/BaseMinigameMode.hpp"

//-----------------------------------------------------------------------------------
struct GladiatorStats : public DefaultPlayerStats
{
    GladiatorStats(PlayerShip* player) : DefaultPlayerStats(player) {};
    virtual ~GladiatorStats() {};

    bool m_isGladiator = false;
};


class GladiatorMinigameMode : public BaseMinigameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    GladiatorMinigameMode();
    virtual ~GladiatorMinigameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize(const std::vector<PlayerShip*>& players);
    virtual void CleanUp();
    virtual void Update(float deltaSeconds);
    virtual void InitializePlayerData() override;
    virtual void RecordPlayerKill(PlayerShip* killer, Ship* victim) override;
    virtual Encounter* GetRandomMinorEncounter(const Vector2& center, float radius) override;
    void SetUpPlayerSpawnPoints();
    void SpawnPlayers();
    void SpawnGeometry();
    void UpdatePlayerScoreDisplay(PlayerShip* player);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};