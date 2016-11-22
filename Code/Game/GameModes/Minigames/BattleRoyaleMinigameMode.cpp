#include "Game/GameModes/Minigames/BattleRoyaleMinigameMode.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Game/Entities/Props/Asteroid.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"

//-----------------------------------------------------------------------------------
BattleRoyaleMinigameMode::BattleRoyaleMinigameMode()
    : BaseMinigameMode()
{
    m_gameLengthSeconds = 5.0f;
    m_enablesRespawn = false;
    m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/SFX/Music/Persona 4 Golden - Time To Make History.mp3");
}

//-----------------------------------------------------------------------------------
BattleRoyaleMinigameMode::~BattleRoyaleMinigameMode()
{

}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::Initialize()
{
    SetBackground("BattleBackground", Vector2(2.0f));
    SpawnGeometry();
    SpawnPlayers();
    m_isPlaying = true;
    GameMode::Initialize();
}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::CleanUp()
{
    for (Entity* ent : m_entities)
    {
        if (!ent->IsPlayer())
        {
            delete ent;
        }
    }
    m_entities.clear();
    GameMode::CleanUp();
}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::SpawnPlayers()
{
    for (PlayerShip* player : TheGame::instance->m_players)
    {
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

    int numPlayersAlive = 0;
    for (unsigned int i = 0; i < TheGame::instance->m_players.size(); ++i)
    {
        SpriteGameRenderer::instance->SetCameraPosition(TheGame::instance->m_players[i]->GetPosition(), i);
        if (!(TheGame::instance->m_players[i]->m_isDead))
        {
            ++numPlayersAlive;
        }
    }
    if (numPlayersAlive < 0)
    {
        m_isPlaying = false;
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