#pragma once
#include "Game/GameModes/BaseMinigameMode.hpp"

//-----------------------------------------------------------------------------------
struct DrainStats : public DefaultPlayerStats
{
    DrainStats(PlayerShip* player) : DefaultPlayerStats(player) {};
    virtual ~DrainStats() {};

    float m_timeAlive = -1.0f;
};

class DrainMinigameMode : public BaseMinigameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    DrainMinigameMode();
    virtual ~DrainMinigameMode();

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
    float m_currentDamagePerInterval = 0.0f;
    float m_currentDrainInterval = 0.0f;
    float m_timeSinceLastDrain = 0.0f;

    //STATIC VARIABLES/////////////////////////////////////////////////////////////////////
    static constexpr float DRAIN_DAMAGE_PER_INTERVAL_MIN = 5.0f;
    static constexpr float DRAIN_DAMAGE_PER_INTERVAL_MAX = 10.0f;
    static constexpr float DRAIN_INTERVAL_MIN = 3.0f;
    static constexpr float DRAIN_INTERVAL_MAX = 0.25f;
};