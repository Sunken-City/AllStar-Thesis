#pragma once
#include "Engine\Renderer\2D\Sprite.hpp"
#include <vector>

class Entity;
class PlayerShip;
class Ship;
class Item;
class Vector2;

class GameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    GameMode(const std::string& arenaBackgroundImage = "DefaultBackground");
    virtual ~GameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize() = 0;
    virtual void CleanUp() = 0;
    virtual void Update(float deltaSeconds);
    virtual Vector2 GetRandomLocationInArena();
    virtual Vector2 GetRandomPlayerSpawnPoint();
    void AddPlayerSpawnPoint(const Vector2& newSpawnPoint);
    AABB2 GetArenaBounds();
    void SpawnBullet(Ship* creator);
    void SpawnPickup(Item* item, const Vector2& spawnPosition);
    void SetBackground(const std::string& backgroundName, const Vector2& scale);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_newEntities;
    bool m_isPlaying = false;
    float m_gameLengthSeconds = 2000.0f;
    bool m_enablesRespawn = true;

private:
    std::vector<Vector2> m_playerSpawnPoints;
    Sprite* m_arenaBackground;
    float m_timerSecondsElapsed = 0.0f;
};