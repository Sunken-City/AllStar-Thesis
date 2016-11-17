#pragma once
#include "Engine\Renderer\2D\Sprite.hpp"
#include <vector>

class Entity;
class PlayerShip;
class Vector2;

class GameMode
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    GameMode(const std::string& arenaBackgroundImage = "DefaultBackground");
    virtual ~GameMode();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Initialize() = 0;
    virtual void Update(float deltaSeconds);
    virtual Vector2 GetRandomLocationInArena();
    virtual Vector2 GetRandomPlayerSpawnPoint();
    void AddPlayerSpawnPoint(const Vector2& newSpawnPoint);
    AABB2 GetArenaBounds();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_newEntities;
    std::vector<PlayerShip*> m_players;
    bool m_isPlaying = false;
    float m_gameLengthSeconds = 2000.0f;

private:
    std::vector<Vector2> m_playerSpawnPoints;
    Sprite m_arenaBackground;
    float m_timerSecondsElapsed = 0.0f;
};