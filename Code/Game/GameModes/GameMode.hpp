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
class Encounter;

//-----------------------------------------------------------------------------------
struct DefaultPlayerStats
{
    DefaultPlayerStats(PlayerShip* player) : m_player(player) {};
    virtual ~DefaultPlayerStats() {};

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
    virtual void Initialize(const std::vector<PlayerShip*>& players);
    virtual void CleanUp();

    void DeleteAllEntities();

    virtual void Update(float deltaSeconds);
    virtual void UpdatePlayerCameras();
    virtual Vector2 GetRandomLocationInArena(float radius = 0.0f);
    virtual Vector2 GetPlayerSpawnPoint(int playerNumber);
    virtual void RemoveEntitiesInCircle(const Vector2& center, float radius);
    virtual void SetUpPlayerSpawnPoints() {};
    void AddPlayerSpawnPoint(const Vector2& newSpawnPoint);
    AABB2 GetArenaBounds();
    virtual void SpawnEntityInGameWorld(Entity* entity);
    virtual void SpawnBullet(Projectile* bullet);
    virtual void SpawnPickup(Item* item, const Vector2& spawnPosition);
    void SetBackground(const std::string& backgroundName, const Vector2& scale);
    void PlaySoundAt(const SoundID sound, const Vector2& soundPosition, float maxVolume = 1.0f);
    float CalculateAttenuation(const Vector2& soundPosition);
    void StopPlaying();
    void MarkTimerPaused();
    virtual void HideBackground();
    virtual void ShowBackground();
    virtual void InitializeReadyAnim();
    virtual void UpdateReadyAnim(float deltaSeconds);
    virtual void CleanupReadyAnim();
    virtual Encounter* GetRandomMinorEncounter(const Vector2& center, float radius);
    virtual Encounter* GetRandomMajorEncounter(const Vector2& center, float radius);
    virtual Vector2 FindSpaceForEncounter(float radius, const std::vector<Encounter*>& encounters);
    virtual void SetTimeRemaining(float timeRemainingSeconds);
    virtual inline float GetTimerSecondsElapsed() { return m_timerSecondsElapsed; };
    virtual inline float GetTimerRealSecondsElapsed() { return m_timerRealSecondsElapsed; };
    void SetVortexPositions();
    void SetVortexPosition(int vortexId, const Vector2& vortexPosition, float radius);
    void ClearVortexPositions();
    inline int GetNextVortexID() { return s_currentVortexId++; };
    virtual void SpawnEncounters();

    //PLAYER DATA/////////////////////////////////////////////////////////////////////
    virtual void InitializePlayerData();
    virtual void RankPlayers();
    virtual void RecordPlayerDeath(PlayerShip* ship);
    virtual void RecordPlayerKill(PlayerShip* killer, Ship* victim);
    virtual void RecordPlayerPickupCoin(PlayerShip*, int) {};

    //STATIC FUNCTIONS/////////////////////////////////////////////////////////////////////
    static GameMode* GetCurrent();
    
    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    static const double AFTER_GAME_SLOWDOWN_SECONDS;
    static const double ANIMATION_LENGTH_SECONDS;
    static const int MAX_NUM_VORTEXES = 16;
    static int s_currentVortexId;

    float MIN_MINOR_RADIUS = 3.0f;
    float MAX_MINOR_RADIUS = 4.0f;
    float MIN_MAJOR_RADIUS = 4.0f;
    float MAX_MAJOR_RADIUS = 5.0f;
    unsigned int MIN_NUM_MINOR_ENCOUNTERS = 2;
    unsigned int MAX_NUM_MINOR_ENCOUNTERS = 4;
    unsigned int MIN_NUM_MAJOR_ENCOUNTERS = 0;
    unsigned int MAX_NUM_MAJOR_ENCOUNTERS = 0;

    std::map<PlayerShip*, DefaultPlayerStats*> m_playerStats;
    std::vector<PlayerShip*> m_players;
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_newEntities;
    std::vector<Encounter*> m_encounters;
    const char* m_modeTitleText;
    const char* m_modeDescriptionText;
    float m_gameLengthSeconds = 2000.0f;
    float m_scaledDeltaSeconds = 0.0f;
    SoundID m_backgroundMusic = 0;
    bool m_respawnAllowed = true;
    bool m_isPlaying = false;
    bool m_dropItemsOnDeath = false;
    bool m_uniquePlayerSpawns = false;
    RGBA m_readyBGColor = RGBA::FOREST_GREEN;
    RGBA m_readyTextColor = RGBA::RED;

private:
    std::vector<Vector2> m_playerSpawnPoints;
    Sprite* m_arenaBackground = nullptr;
    Sprite* m_starfield = nullptr;
    Sprite* m_starfield2 = nullptr;
    float m_timerSecondsElapsed = 0.0f;
    float m_timerRealSecondsElapsed = 0.0f;
    WidgetBase* m_timerWidget = nullptr;
    WidgetBase* m_countdownWidget = nullptr;

    Material* m_readyAnimFBOEffect = nullptr;
    TextRenderable2D* m_modeTitleRenderable = nullptr;
    TextRenderable2D* m_getReadyRenderable = nullptr;
};