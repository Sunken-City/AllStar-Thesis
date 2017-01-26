#pragma once
#include "Engine\Renderer\2D\Sprite.hpp"
#include "Engine\Audio\Audio.hpp"
#include <vector>

class Entity;
class PlayerShip;
class Ship;
class Item;
class Vector2;
class WidgetBase;
class Projectile;

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
    void SpawnBullet(Projectile* bullet);
    void SpawnPickup(Item* item, const Vector2& spawnPosition);
    void SetBackground(const std::string& backgroundName, const Vector2& scale);
    void PlaySoundAt(const SoundID sound, const Vector2& soundPosition, float maxVolume = 1.0f);
    float CalculateAttenuation(const Vector2& soundPosition);
    void StopPlaying();
    
    static GameMode* GetCurrent();
    
    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_newEntities;
    float m_gameLengthSeconds = 2000.0f;
    bool m_enablesRespawn = true;
    SoundID m_backgroundMusic = 0;
    bool m_muteMusic = true;
    bool m_isPlaying = false;
    bool m_dropItemsOnDeath = false;

private:
    std::vector<Vector2> m_playerSpawnPoints;
    Sprite* m_arenaBackground = nullptr;
    Sprite* m_starfield = nullptr;
    Sprite* m_starfield2 = nullptr;
    float m_timerSecondsElapsed = 0.0f;
    WidgetBase* m_timerWidget = nullptr;
};