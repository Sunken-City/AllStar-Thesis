#pragma once
#include "Game/GameModes/BaseMinigameMode.hpp"

//-----------------------------------------------------------------------------------
struct OuroborosPlayerStats : public DefaultPlayerStats
{
    OuroborosPlayerStats(PlayerShip* player) : DefaultPlayerStats(player) {};
    virtual ~OuroborosPlayerStats() {};

    int m_numberOfCoins = 0;
};

//-----------------------------------------------------------------------------------
class OuroborosMinigameMode : public BaseMinigameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    OuroborosMinigameMode();
    virtual ~OuroborosMinigameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize(const std::vector<PlayerShip*>& players);
    virtual void CleanUp();
    virtual void Update(float deltaSeconds);
    virtual Encounter* GetRandomMinorEncounter(const Vector2& center, float radius) override;
    void SetUpPlayerSpawnPoints();
    void SpawnPlayers();
    void SpawnGeometry();
    void UpdatePlayerScoreDisplay(PlayerShip* player);
    virtual void RecordPlayerDeath(PlayerShip* ship);
    virtual void RecordPlayerPickupCoin(PlayerShip* ship, int coinValue);
    virtual void InitializePlayerData() override;
    virtual void RankPlayers() override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static constexpr float SECONDS_BETWEEN_COIN_SPAWNS = 1.0f;
    float m_timeSinceLastCoin = 0.0f;
};
