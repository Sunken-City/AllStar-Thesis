#include "Game/GameModes/Minigames/BattleRoyaleMinigameMode.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Game/Entities/Props/Asteroid.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Engine/Input/XInputController.hpp"

//-----------------------------------------------------------------------------------
BattleRoyaleMinigameMode::BattleRoyaleMinigameMode()
    : BaseMinigameMode()
{
    m_gameLengthSeconds = 121.0f;
    m_enablesRespawn = false;
    m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/SFX/Music/Persona 4 Golden - Time To Make History.mp3");
    m_modeTitleText = "BATTLE ROYALE";
}

//-----------------------------------------------------------------------------------
BattleRoyaleMinigameMode::~BattleRoyaleMinigameMode()
{

}

//-----------------------------------------------------------------------------------
void BattleRoyaleMinigameMode::Initialize()
{
    SetBackground("BattleBackground", Vector2(50.0f));
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 10.0f;
    SpriteGameRenderer::instance->SetWorldBounds(AABB2(Vector2(-20.0f), Vector2(20.0f)));
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
        player->Respawn();
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
    deltaSeconds = m_scaledDeltaSeconds;

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
    for (auto iter = m_entities.begin(); iter != m_entities.end();)
    {
        Entity* gameObject = *iter;
        if (gameObject->m_isDead && !gameObject->IsPlayer())
        {
            delete gameObject;
            iter = m_entities.erase(iter);
            continue;
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

        float aimingDeadzoneThreshold = XInputController::INNER_DEADZONE;
        float aimingDeadzoneThresholdSquared = aimingDeadzoneThreshold * aimingDeadzoneThreshold;
        if (playerRightStick.CalculateMagnitudeSquared() > aimingDeadzoneThresholdSquared)
        {
            targetCameraPosition += playerRightStick;
        }

        Vector2 currentCameraPosition = SpriteGameRenderer::instance->GetCameraPositionInWorld(i);
        Vector2 cameraPosition = MathUtils::Lerp(0.1f, currentCameraPosition, targetCameraPosition);
        SpriteGameRenderer::instance->SetCameraPosition(cameraPosition, i);
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