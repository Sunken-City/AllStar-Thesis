#include "Game/GameModes/Minigames/BattleRoyaleMinigameMode.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Game/Entities/Props/Asteroid.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"

//-----------------------------------------------------------------------------------
BattleRoyaleMinigameMode::BattleRoyaleMinigameMode()
    : BaseMinigameMode()
{
    SetBackground("BattleBackground", Vector2(2.0f));
    m_gameLengthSeconds = 60.0f;
}

//-----------------------------------------------------------------------------------
BattleRoyaleMinigameMode::~BattleRoyaleMinigameMode()
{

}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::Initialize()
{
    SpawnGeometry();
    SpawnPlayers();
}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::SpawnPlayers()
{
    for (unsigned int i = 0; i < TheGame::instance->m_playerPilots.size(); ++i)
    {
        PlayerShip* player = new PlayerShip(TheGame::instance->m_playerPilots[i]);
        player->SetPosition(GetRandomPlayerSpawnPoint());
        m_players.push_back(player);
        m_entities.push_back(player);
    }
}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::SpawnGeometry()
{
    //Add in some Asteroids (for color)
    for (int i = 0; i < 20; ++i)
    {
        m_entities.push_back(new Asteroid(GetRandomLocationInArena()));
    }

    m_isPlaying = true;
}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::Update(float deltaSeconds)
{
    BaseMinigameMode::Update(deltaSeconds);
    if (!m_isPlaying)
    {
        return;
    }
    for (Entity* ent : m_entities)
    {
        ent->Update(deltaSeconds);
        for (Entity* other : m_entities)
        {
            if ((ent != other) && (ent->IsCollidingWith(other)))
            {
                ent->ResolveCollision(other);
            }
        }
    }
    for (Entity* ent : m_newEntities)
    {
        m_entities.push_back(ent);
    }
    m_newEntities.clear();
    for (auto iter = m_entities.begin(); iter != m_entities.end(); ++iter)
    {
        Entity* gameObject = *iter;
        if (gameObject->m_isDead && !gameObject->IsPlayer())
        {
            delete gameObject;
            iter = m_entities.erase(iter);
        }
        if (iter == m_entities.end())
        {
            break;
        }
    }

    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        SpriteGameRenderer::instance->SetCameraPosition(m_players[i]->GetPosition(), i);
    }
}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::SetUpPlayerSpawnPoints()
{
    AABB2 bounds = GetArenaBounds();
    AddPlayerSpawnPoint(Vector2::ZERO);
    AddPlayerSpawnPoint(bounds.mins + Vector2::ONE);
    AddPlayerSpawnPoint(bounds.mins + Vector2(2.0f));
    AddPlayerSpawnPoint(bounds.maxs - Vector2::ONE);
    AddPlayerSpawnPoint(bounds.maxs - Vector2(2.0f));
}