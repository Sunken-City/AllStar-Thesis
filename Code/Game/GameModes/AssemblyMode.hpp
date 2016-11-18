#pragma once
#include "Game/GameModes/GameMode.hpp"

class AssemblyMode : public GameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    AssemblyMode();
    virtual ~AssemblyMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize();
    virtual void CleanUp();
    void SpawnStartingEntities();
    void SpawnPlayers();
    void SpawnGeometry();
    virtual void Update(float deltaSeconds);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
private:
    float m_timeSinceLastSpawn = 0.0f;
    const float TIME_PER_SPAWN = 5.0f;
};