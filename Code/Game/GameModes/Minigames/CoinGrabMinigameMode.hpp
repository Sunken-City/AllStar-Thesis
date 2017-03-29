#pragma once
#include "Game/GameModes/BaseMinigameMode.hpp"

//-----------------------------------------------------------------------------------
struct CoinGrabPlayerStats : public DefaultPlayerStats
{
    CoinGrabPlayerStats(PlayerShip* player) : DefaultPlayerStats(player) {};
    virtual ~CoinGrabPlayerStats() {};

    int m_numberOfCoins = 0;
};

//-----------------------------------------------------------------------------------
class CoinGrabMinigameMode : public BaseMinigameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    CoinGrabMinigameMode();
    virtual ~CoinGrabMinigameMode();

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
    static constexpr float SECONDS_BETWEEN_COIN_SPAWNS = 0.5f;
    float m_timeSinceLastCoin = 0.0f;
};
