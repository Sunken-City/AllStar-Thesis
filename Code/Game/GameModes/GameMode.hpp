#pragma once
#include "Engine\Renderer\2D\Sprite.hpp"
#include "Engine\Audio\Audio.hpp"
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
    virtual void Initialize();
    virtual void CleanUp();
    virtual void Update(float deltaSeconds);
    virtual Vector2 GetRandomLocationInArena();
    virtual Vector2 GetRandomPlayerSpawnPoint();
    void AddPlayerSpawnPoint(const Vector2& newSpawnPoint);
    AABB2 GetArenaBounds();
    void SpawnBullet(Ship* creator);
    void SpawnPickup(Item* item, const Vector2& spawnPosition);
    void SetBackground(const std::string& backgroundName, const Vector2& scale);
    float CalculateAttenuation(const Vector2& soundPosition);
    void PlaySoundAt(const SoundID sound, const Vector2& soundPosition, float maxVolume = 1.0f);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_newEntities;
    bool m_isPlaying = false;
    float m_gameLengthSeconds = 2000.0f;
    bool m_enablesRespawn = true;
    SoundID m_backgroundMusic = 0;

private:
    std::vector<Vector2> m_playerSpawnPoints;
    Sprite* m_arenaBackground;
    float m_timerSecondsElapsed = 0.0f;
};