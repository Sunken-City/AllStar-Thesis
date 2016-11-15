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
    AABB2 GetArenaBounds();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
public:
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_newEntities;
    std::vector<PlayerShip*> m_players;
    bool m_isPlaying = false;
    float m_gameLengthSeconds = 200.0f;

private:
    Sprite m_arenaBackground;
    float m_timerSecondsElapsed = 0.0f;
};