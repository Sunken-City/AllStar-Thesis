#include "Game/GameModes/AssemblyMode.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Game/TheGame.hpp"
#include "Game/Pilots/PlayerPilot.hpp"
#include "Game/Entities/Props/ItemCrate.hpp"
#include "Game/Entities/Grunt.hpp"
#include "Game/Entities/Props/Asteroid.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"

//-----------------------------------------------------------------------------------
AssemblyMode::AssemblyMode()
    : GameMode()
{
    SetBackground("Assembly", Vector2(70.0f));
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 20.0f;
    m_gameLengthSeconds = 301.0f;
    m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/SFX/Music/Foxx - Sweet Tooth - 02 Jawbreaker.flac");
}

//-----------------------------------------------------------------------------------
AssemblyMode::~AssemblyMode()
{
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 1.0f;
}

//-----------------------------------------------------------------------------------
void AssemblyMode::Initialize()
{
    m_isPlaying = true;
    SpawnGeometry();
    SpawnStartingEntities();
    SpawnPlayers();
    GameMode::Initialize();
}

//-----------------------------------------------------------------------------------
void AssemblyMode::CleanUp()
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
void AssemblyMode::SpawnStartingEntities()
{
    if (g_spawnEnemies && g_spawnCrates)
    {
        ItemCrate* box1 = new ItemCrate(Vector2(2.0f));
        ItemCrate* box2 = new ItemCrate(Vector2(1.0f));
        Grunt* g1 = new Grunt(Vector2(-2.0f));
        Grunt* g2 = new Grunt(Vector2(-1.0f));

        m_entities.push_back(box1);
        m_entities.push_back(box2);
        m_entities.push_back(g1);
        m_entities.push_back(g2);
    }
}

//-----------------------------------------------------------------------------------
void AssemblyMode::SpawnPlayers()
{
    for (unsigned int i = 0; i < TheGame::instance->m_playerPilots.size(); ++i)
    {
        PlayerShip* player = new PlayerShip(TheGame::instance->m_playerPilots[i]);
        player->SetPosition(GetRandomPlayerSpawnPoint());
        TheGame::instance->m_players.push_back(player);
        m_entities.push_back(player);
    }
}

//-----------------------------------------------------------------------------------
void AssemblyMode::SpawnGeometry()
{
    if (!g_spawnGeometry)
    {
        return;
    }
    //Add in some Asteroids (for color)
    for (int i = 0; i < 20; ++i)
    {
        m_entities.push_back(new Asteroid(GetRandomLocationInArena()));
    }
}

//-----------------------------------------------------------------------------------
void AssemblyMode::Update(float deltaSeconds)
{
    GameMode::Update(deltaSeconds);
    if (!m_isPlaying)
    {
        return;
    }
    m_timeSinceLastSpawn += deltaSeconds;
    if (g_spawnEnemies && g_spawnCrates && m_timeSinceLastSpawn > TIME_PER_SPAWN)
    {
        m_entities.push_back(new ItemCrate(GetRandomLocationInArena()));
        m_entities.push_back(new Grunt(GetRandomLocationInArena()));
        m_timeSinceLastSpawn = 0.0f;
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

    for (unsigned int i = 0; i < TheGame::instance->m_players.size(); ++i)
    {
        SpriteGameRenderer::instance->SetCameraPosition(TheGame::instance->m_players[i]->GetPosition(), i);
    }
}