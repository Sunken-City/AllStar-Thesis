#include "Game/GameModes/AssemblyMode.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Game/TheGame.hpp"
#include "Game/Pilots/PlayerPilot.hpp"
#include "Game/Entities/Props/ItemCrate.hpp"
#include "Game/Entities/Enemies/Grunt.hpp"
#include "Game/Entities/Props/Asteroid.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Engine/Input/XInputController.hpp"
#include "../Encounters/SquadronEncounter.hpp"
#include "../Encounters/NebulaEncounter.hpp"
#include "../Encounters/WormholeEncounter.hpp"
#include "../Encounters/BlackHoleEncounter.hpp"
#include "../Encounters/CargoShipEncounter.hpp"
#include "../Encounters/BossteroidEncounter.hpp"

//-----------------------------------------------------------------------------------
AssemblyMode::AssemblyMode()
    : GameMode()
{
    SetBackground("Assembly", Vector2(25.0f));
    m_modeTitleText = "ASSEMBLY MODE";
    m_modeDescriptionText = "Collect and Prepare for Challenges!";
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 5.0f;
    m_gameLengthSeconds = 300.0f;
    m_dropItemsOnDeath = true;
    if (!g_disableMusic)
    {
        m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Sweet Tooth - 02 Jawbreaker.ogg");
    }

    MIN_NUM_MINOR_ENCOUNTERS = 8;
    MAX_NUM_MINOR_ENCOUNTERS = 12;
    MIN_NUM_MAJOR_ENCOUNTERS = 3;
    MAX_NUM_MAJOR_ENCOUNTERS = 6;
}

//-----------------------------------------------------------------------------------
AssemblyMode::~AssemblyMode()
{
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 1.0f;
}

//-----------------------------------------------------------------------------------
void AssemblyMode::Initialize(const std::vector<PlayerShip*>& players)
{
    m_isPlaying = true;
    SpriteGameRenderer::instance->SetWorldBounds(AABB2(Vector2(-WORLD_SIZE, -WORLD_SIZE), Vector2(WORLD_SIZE, WORLD_SIZE)));
    GameMode::Initialize(players);
    GenerateLevel();
    SpawnStartingEntities();
    SpawnPlayers();
}

//-----------------------------------------------------------------------------------
void AssemblyMode::CleanUp()
{
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
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        PlayerShip* player = m_players[i];
        player->SetPosition(GetPlayerSpawnPoint(i));
        player->FlushParticleTrailIfExists();
        player->Respawn();
        m_entities.push_back(player);
    }
    InitializePlayerData();
}

//-----------------------------------------------------------------------------------
void AssemblyMode::GenerateLevel()
{
    if (!g_spawnGeometry)
    {
        return;
    }
    FillMapWithAsteroids();
    SpawnEncounters();
}

//-----------------------------------------------------------------------------------
Encounter* AssemblyMode::GetRandomMinorEncounter(const Vector2& center, float radius)
{
    int random = MathUtils::GetRandomIntFromZeroTo(4);
    switch (random)
    {
    case 0:
    case 1:
        return new NebulaEncounter(center, radius);
    case 2:
        return new CargoShipEncounter(center, radius);
    case 3:
        return new BossteroidEncounter(center, radius);
    default:
        ERROR_AND_DIE("Random medium encounter roll out of range");
    }
}

//-----------------------------------------------------------------------------------
void AssemblyMode::FillMapWithAsteroids()
{
    unsigned int numAsteroids = MathUtils::GetRandomInt(MIN_NUM_ASTEROIDS, MAX_NUM_ASTEROIDS);

    for (int i = 0; i < numAsteroids; ++i)
    {
        m_entities.push_back(new Asteroid(GetRandomLocationInArena()));
    }
}

//-----------------------------------------------------------------------------------
void AssemblyMode::Update(float deltaSeconds)
{
    GameMode::Update(deltaSeconds);
    deltaSeconds = m_scaledDeltaSeconds;

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
    for (auto iter = m_entities.begin(); iter != m_entities.end();)
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
        ++iter;
    }

    UpdatePlayerCameras();
}