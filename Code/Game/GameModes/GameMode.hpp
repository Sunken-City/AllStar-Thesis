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
    virtual void Update(float deltaSeconds);
    virtual void UpdatePlayerCameras();
    virtual Vector2 GetRandomLocationInArena(float radius = 0.0f);
    virtual Vector2 GetRandomPlayerSpawnPoint();
    virtual void RemoveEntitiesInCircle(const Vector2& center, float radius);
    void AddPlayerSpawnPoint(const Vector2& newSpawnPoint);
    AABB2 GetArenaBounds();
    virtual void SpawnEntityInGameWorld(Entity* entity);
    virtual void SpawnBullet(Projectile* bullet);
    virtual void SpawnPickup(Item* item, const Vector2& spawnPosition);
    void SetBackground(const std::string& backgroundName, const Vector2& scale);
    void PlaySoundAt(const SoundID sound, const Vector2& soundPosition, float maxVolume = 1.0f);
    float CalculateAttenuation(const Vector2& soundPosition);
    void StopPlaying();
    virtual void HideBackground();
    virtual void ShowBackground();
    virtual void InitializeReadyAnim();
    virtual void UpdateReadyAnim(float deltaSeconds);
    virtual void CleanupReadyAnim();
    virtual Encounter* GetRandomMediumEncounter(const Vector2& center, float radius);
    virtual Encounter* GetRandomLargeEncounter(const Vector2& center, float radius);
    virtual Vector2 FindSpaceForEncounter(float radius, const std::vector<Encounter*>& encounters);
    virtual void SetTimeRemaining(float timeRemainingSeconds);
    virtual inline float GetTimerSecondsElapsed() { return m_timerSecondsElapsed; };

    //PLAYER DATA/////////////////////////////////////////////////////////////////////
    virtual void InitializePlayerData();
    virtual void RecordPlayerDeath(PlayerShip* ship);
    virtual void RecordPlayerKill(PlayerShip* killer, Ship* victim);
    virtual void DetermineWinners();

    //STATIC FUNCTIONS/////////////////////////////////////////////////////////////////////
    static GameMode* GetCurrent();
    
    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    static const double AFTER_GAME_SLOWDOWN_SECONDS;
    static const double ANIMATION_LENGTH_SECONDS;

    std::map<PlayerShip*, DefaultPlayerStats*> m_playerStats;
    std::vector<PlayerShip*> m_players;
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_newEntities;
    const char* m_modeTitleText;
    const char* m_modeDescriptionText;
    float m_gameLengthSeconds = 2000.0f;
    float m_scaledDeltaSeconds = 0.0f;
    SoundID m_backgroundMusic = 0;
    bool m_respawnAllowed = true;
    bool m_isPlaying = false;
    bool m_dropItemsOnDeath = false;
    RGBA m_readyBGColor = RGBA::FOREST_GREEN;
    RGBA m_readyTextColor = RGBA::RED;

private:
    std::vector<Vector2> m_playerSpawnPoints;
    Sprite* m_arenaBackground = nullptr;
    Sprite* m_starfield = nullptr;
    Sprite* m_starfield2 = nullptr;
    float m_timerSecondsElapsed = 0.0f;
    WidgetBase* m_timerWidget = nullptr;
    WidgetBase* m_countdownWidget = nullptr;

    Material* m_readyAnimFBOEffect = nullptr;
    TextRenderable2D* m_modeTitleRenderable = nullptr;
    TextRenderable2D* m_getReadyRenderable = nullptr;
};