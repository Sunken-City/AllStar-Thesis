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

//-----------------------------------------------------------------------------------
AssemblyMode::AssemblyMode()
    : GameMode()
{
    SetBackground("Assembly", Vector2(15.0f));
    m_modeTitleText = "ASSEMBLY MODE";
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 5.0f;
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
void AssemblyMode::Initialize()
{
    m_isPlaying = true;
    GenerateLevel();
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

    int numEncounters = MathUtils::GetRandomInt(MIN_NUM_MEDIUM_ENCOUNTERS, MAX_NUM_MEDIUM_ENCOUNTERS);
    std::vector<Encounter*> encounters;

    for (int i = 0; i < numEncounters; ++i)
    {
        bool succeeded = false;
        do 
        {
            float radius = MathUtils::GetRandomFloat(MIN_MEDIUM_RADIUS, MAX_MEDIUM_RADIUS);
            Vector2 center = GetRandomLocationInArena();
            Encounter* newEncounter = GetRandomMediumEncounter(center, radius);

            if (encounters.size() == 0)
            {
                succeeded = true;
                RemoveEntitiesInCircle(center, radius);
                encounters.push_back(newEncounter);
                newEncounter->Spawn();
            }

            for (Encounter* encounter : encounters)
            {
                float combinedDistance = radius + encounter->m_radius;
                float combinedDistanceSquared = combinedDistance * combinedDistance;
                if (MathUtils::CalcDistSquaredBetweenPoints(encounter->m_center, center) > combinedDistanceSquared)
                {
                    succeeded = true;
                    RemoveEntitiesInCircle(center, radius);
                    encounters.push_back(newEncounter);
                    newEncounter->Spawn();
                    break;
                }
            }
        } while (!succeeded);
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
Encounter* AssemblyMode::GetRandomMediumEncounter(const Vector2& center, float radius)
{
    int random = MathUtils::GetRandomIntFromZeroTo(2);
    switch (random)
    {
    case 0:
        return new SquadronEncounter(center, radius);
    case 1:
        return new NebulaEncounter(center, radius);
    default:
        ERROR_AND_DIE("Random medium encounter roll out of range");
    }
}

//-----------------------------------------------------------------------------------
Encounter* AssemblyMode::GetRandomLargeEncounter(const Vector2& center, float radius)
{
    int random = MathUtils::GetRandomIntFromZeroTo(2);
    switch (random)
    {
    case 0:
        return new SquadronEncounter(center, radius);
    case 1:
        return new NebulaEncounter(center, radius);
    default:
        ERROR_AND_DIE("Random medium encounter roll out of range");
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

    for (unsigned int i = 0; i < TheGame::instance->m_players.size(); ++i)
    {
        PlayerShip* player = TheGame::instance->m_players[i];
        Vector2 targetCameraPosition = player->GetPosition();
        Vector2 playerRightStick = player->m_pilot->m_inputMap.GetVector2("ShootRight", "ShootUp");

        if (InputSystem::instance->WasKeyJustPressed('R'))
        {
            float radius = 5.0f;
            RemoveEntitiesInCircle(player->m_transform.GetWorldPosition(), radius);
            WormholeEncounter nebby(player->m_transform.GetWorldPosition(), radius);
            nebby.Spawn();
        }

        float aimingDeadzoneThreshold = XInputController::INNER_DEADZONE;
        float aimingDeadzoneThresholdSquared = aimingDeadzoneThreshold * aimingDeadzoneThreshold;
        if (playerRightStick.CalculateMagnitudeSquared() > aimingDeadzoneThresholdSquared)
        {
            targetCameraPosition += playerRightStick;
        }
        if (player->IsDead())
        {
            targetCameraPosition = player->GetPosition();
        }

        Vector2 currentCameraPosition = SpriteGameRenderer::instance->GetCameraPositionInWorld(i);
        Vector2 cameraPosition = MathUtils::Lerp(0.1f, currentCameraPosition, targetCameraPosition);
        SpriteGameRenderer::instance->SetCameraPosition(cameraPosition, i);
    }
}