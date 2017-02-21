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
    void GenerateLevel();

    void SpawnEncounters();

    void FillMapWithAsteroids();
    virtual void Update(float deltaSeconds);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
private:
    float m_timeSinceLastSpawn = 0.0f;
    const float TIME_PER_SPAWN = 5.0f;
    const float WORLD_SIZE = 40.0f;
    const float MIN_MEDIUM_RADIUS = 3.0f;
    const float MAX_MEDIUM_RADIUS = 4.0f;
    const float MIN_LARGE_RADIUS = 4.0f;
    const float MAX_LARGE_RADIUS = 5.0f;
    static const unsigned int MIN_NUM_ASTEROIDS = 40;
    static const unsigned int MAX_NUM_ASTEROIDS = 80;
    static const unsigned int MIN_NUM_MEDIUM_ENCOUNTERS = 8;
    static const unsigned int MAX_NUM_MEDIUM_ENCOUNTERS = 12;
    static const unsigned int MIN_NUM_LARGE_ENCOUNTERS = 3;
    static const unsigned int MAX_NUM_LARGE_ENCOUNTERS = 6;
};