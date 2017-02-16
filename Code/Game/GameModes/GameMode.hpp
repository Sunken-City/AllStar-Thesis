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
class TextRenderable2D;

//-----------------------------------------------------------------------------------
struct DefaultPlayerStats
{
    DefaultPlayerStats(PlayerShip* player) : m_player(player) {};

    PlayerShip* m_player = nullptr;
    int m_numKills = 0;
    int m_numDeaths = 0;
};

//-----------------------------------------------------------------------------------
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
    virtual void RemoveEntitiesInCircle(const Vector2& center, float radius);
    void AddPlayerSpawnPoint(const Vector2& newSpawnPoint);
    AABB2 GetArenaBounds();
    void SpawnBullet(Projectile* bullet);
    void SpawnPickup(Item* item, const Vector2& spawnPosition);
    void SetBackground(const std::string& backgroundName, const Vector2& scale);
    void PlaySoundAt(const SoundID sound, const Vector2& soundPosition, float maxVolume = 1.0f);
    float CalculateAttenuation(const Vector2& soundPosition);
    void StopPlaying();
    void HideBackground();
    void ShowBackground();
    virtual void InitializeReadyAnim();
    virtual void UpdateReadyAnim(float deltaSeconds);
    virtual void CleanupReadyAnim();

    //PLAYER DATA/////////////////////////////////////////////////////////////////////
    virtual void InitializePlayerData();
    virtual void RecordPlayerDeath(PlayerShip* ship);
    virtual void RecordPlayerKill(PlayerShip* killer, Ship* victim);
    virtual void DetermineWinners();
    
    static GameMode* GetCurrent();
    
    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_newEntities;
    const char* m_modeTitleText;
    float m_gameLengthSeconds = 2000.0f;
    bool m_enablesRespawn = true;
    SoundID m_backgroundMusic = 0;
    bool m_muteMusic = true;
    bool m_isPlaying = false;
    bool m_dropItemsOnDeath = false;
    std::map<PlayerShip*, DefaultPlayerStats*> m_playerStats;

private:
    std::vector<Vector2> m_playerSpawnPoints;
    Sprite* m_arenaBackground = nullptr;
    Sprite* m_starfield = nullptr;
    Sprite* m_starfield2 = nullptr;
    float m_timerSecondsElapsed = 0.0f;
    WidgetBase* m_timerWidget = nullptr;

    float m_rotationTime = 0.0f;
    TextRenderable2D* m_modeTitleRenderable = nullptr;
    TextRenderable2D* m_getReadyRenderable = nullptr;
    Transform2D m_leftSpindleCenter;
    Transform2D m_rightSpindleCenter;
    Sprite* m_leftSpindles[3];
    Sprite* m_rightSpindles[3];
};