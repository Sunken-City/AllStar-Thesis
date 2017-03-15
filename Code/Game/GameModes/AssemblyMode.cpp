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
#include "Engine/Input/XInputController.hpp"
#include "../Encounters/SquadronEncounter.hpp"
#include "../Encounters/NebulaEncounter.hpp"
#include "../Encounters/WormholeEncounter.hpp"
#include "../Encounters/BlackHoleEncounter.hpp"

//-----------------------------------------------------------------------------------
AssemblyMode::AssemblyMode()
    : GameMode()
{
    SetBackground("Assembly", Vector2(25.0f));
    m_modeTitleText = "ASSEMBLY MODE";
    m_modeDescriptionText = "Collect and Prepare for Challenges!";
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 10.0f;
    m_gameLengthSeconds = 301.0f;
    m_dropItemsOnDeath = true;
    m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/SFX/Music/Foxx - Sweet Tooth - 02 Jawbreaker.flac");
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
    for (PlayerShip* player : m_players)
    {
        player->SetPosition(GetRandomPlayerSpawnPoint());
        player->FlushParticleTrailIfExists();
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
void AssemblyMode::SpawnEncounters()
{
    int numMediumEncounters = MathUtils::GetRandomInt(MIN_NUM_MEDIUM_ENCOUNTERS, MAX_NUM_MEDIUM_ENCOUNTERS);
    int numLargeEncounters = MathUtils::GetRandomInt(MIN_NUM_LARGE_ENCOUNTERS, MAX_NUM_LARGE_ENCOUNTERS);
    std::vector<Encounter*> encounters;

    for (int i = 0; i < numLargeEncounters; ++i)
    {
        float radius = MathUtils::GetRandomFloat(MIN_LARGE_RADIUS, MAX_LARGE_RADIUS);
        Vector2 center = FindSpaceForEncounter(radius, encounters);
        Encounter* newEncounter = GetRandomLargeEncounter(center, radius);

        RemoveEntitiesInCircle(center, radius);
        encounters.push_back(newEncounter);
        newEncounter->Spawn();

        if (newEncounter->NeedsLinkedEncounter())
        {
            float linkedRadius = MathUtils::GetRandomFloat(MIN_LARGE_RADIUS, MAX_LARGE_RADIUS);
            Vector2 linkedCenter = FindSpaceForEncounter(linkedRadius, encounters);
            Encounter* linkedEncounter = newEncounter->CreateLinkedEncounter(linkedCenter, linkedRadius);

            RemoveEntitiesInCircle(linkedCenter, linkedRadius);
            encounters.push_back(linkedEncounter);
            linkedEncounter->Spawn();
            ++i;
        }
    }

    for (int i = 0; i < numMediumEncounters; ++i)
    {
        float radius = MathUtils::GetRandomFloat(MIN_MEDIUM_RADIUS, MAX_MEDIUM_RADIUS);
        Vector2 center = FindSpaceForEncounter(radius, encounters);
        Encounter* newEncounter = GetRandomMediumEncounter(center, radius);

        RemoveEntitiesInCircle(center, radius);
        encounters.push_back(newEncounter);
        newEncounter->Spawn();

        if (newEncounter->NeedsLinkedEncounter())
        {
            float linkedRadius = MathUtils::GetRandomFloat(MIN_MEDIUM_RADIUS, MAX_MEDIUM_RADIUS);
            Vector2 linkedCenter = FindSpaceForEncounter(linkedRadius, encounters);
            Encounter* linkedEncounter = newEncounter->CreateLinkedEncounter(linkedCenter, linkedRadius);

            RemoveEntitiesInCircle(linkedCenter, linkedRadius);
            encounters.push_back(linkedEncounter);
            linkedEncounter->Spawn();
            ++i;
        }
    }

    for (Encounter* encounter : encounters)
    {
        delete encounter;
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