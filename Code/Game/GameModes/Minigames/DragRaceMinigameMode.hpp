#pragma once
#include "Game/GameModes/BaseMinigameMode.hpp"

class BarGraphRenderable2D;

//-----------------------------------------------------------------------------------
struct RacePlayerStats : public DefaultPlayerStats
{
    RacePlayerStats(PlayerShip* player) : DefaultPlayerStats(player) {};
    virtual ~RacePlayerStats() {};

    float m_distanceToFinish = 0.0f;
    float m_timeToFinish = -1.0f;
};

class DragRaceMinigameMode : public BaseMinigameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    DragRaceMinigameMode();
    virtual ~DragRaceMinigameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize(const std::vector<PlayerShip*>& players);
    virtual void CleanUp();
    virtual void Update(float deltaSeconds);
    virtual Encounter* GetRandomMinorEncounter(const Vector2& center, float radius) override;
    virtual void SetUpPlayerSpawnPoints() override;
    void SpawnPlayers();
    void SpawnGeometry();
    void UpdatePlayerScoreDisplay(PlayerShip* player);
    virtual void InitializePlayerData() override;
    virtual void RankPlayers() override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static constexpr float ARENA_HEIGHT = 160.0f;
    static constexpr float ARENA_WIDTH = 18.0f;
    static constexpr float HALF_ARENA_HEIGHT = ARENA_HEIGHT * 0.5f;
    static constexpr float HALF_ARENA_WIDTH = ARENA_WIDTH * 0.5f;
    static constexpr float FINISH_LINE_HEIGHT = 3.0f;

    AABB2 m_winZone;
    BarGraphRenderable2D* m_winZoneRenderer = nullptr;
};
