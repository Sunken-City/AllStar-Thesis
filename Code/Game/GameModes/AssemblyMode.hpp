#pragma once
#include "Game/GameModes/GameMode.hpp"

class AssemblyMode : public GameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    AssemblyMode();
    virtual ~AssemblyMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize(const std::vector<PlayerShip*>& players);
    virtual void CleanUp();
    void SpawnStartingEntities();
    void SpawnPlayers();
    void GenerateLevel();
    virtual Encounter* GetRandomMinorEncounter(const Vector2& center, float radius) override;

    void FillMapWithAsteroids();
    virtual void Update(float deltaSeconds);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
private:
    static const unsigned int MIN_NUM_ASTEROIDS = 40;
    static const unsigned int MAX_NUM_ASTEROIDS = 80;
    const float TIME_PER_SPAWN = 5.0f;
    const float WORLD_SIZE = 40.0f;

    float m_timeSinceLastSpawn = 0.0f;
};