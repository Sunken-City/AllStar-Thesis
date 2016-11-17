#pragma once
#include "Game/GameModes/GameMode.hpp"

class AssemblyMode : public GameMode
{
public:
    AssemblyMode();
    virtual ~AssemblyMode();

    virtual void Initialize();

    void SpawnStartingEntities();

    void SpawnPlayers();

    void SpawnGeometry();

    virtual void Update(float deltaSeconds);
    void SetUpPlayerSpawnPoints();
private:
    float m_timeSinceLastSpawn = 0.0f;
    const float TIME_PER_SPAWN = 5.0f;
};