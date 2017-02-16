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
    void FillMapWithAsteroids();
    virtual Encounter* GetRandomMediumEncounter(const Vector2& center, float radius);
    virtual Encounter* GetRandomLargeEncounter(const Vector2& center, float radius);

    virtual void Update(float deltaSeconds);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
private:
    float m_timeSinceLastSpawn = 0.0f;
    const float TIME_PER_SPAWN = 5.0f;
    const unsigned int MIN_NUM_ASTEROIDS = 20;
    const unsigned int MAX_NUM_ASTEROIDS = 40;
    const unsigned int MIN_NUM_MEDIUM_ENCOUNTERS = 6;
    const unsigned int MAX_NUM_MEDIUM_ENCOUNTERS = 10;
    const unsigned int MIN_NUM_LARGE_ENCOUNTERS = 0;
    const unsigned int MAX_NUM_LARGE_ENCOUNTERS = 0;
    const float MIN_MEDIUM_RADIUS = 2.0f;
    const float MAX_MEDIUM_RADIUS = 5.0f;
};