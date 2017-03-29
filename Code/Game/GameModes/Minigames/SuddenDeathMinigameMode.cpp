#include "Game/GameModes/Minigames/SuddenDeathMinigameMode.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Game/Entities/Props/Asteroid.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Engine/Input/XInputController.hpp"
#include "Game/Encounters/Encounter.hpp"
#include "Game/Encounters/NebulaEncounter.hpp"
#include "Game/Encounters/BossteroidEncounter.hpp"
#include "Engine/Renderer/2D/TextRenderable2D.hpp"
#include "Game/Encounters/BlackHoleEncounter.hpp"
#include "Game/Entities/Props/BlackHole.hpp"

//-----------------------------------------------------------------------------------
SuddenDeathMinigameMode::SuddenDeathMinigameMode()
    : BaseMinigameMode()
{
    m_gameLengthSeconds = 120.0f;
    m_respawnAllowed = false;
    if (!g_disableMusic)
    {
        m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Function - 07 PROJECT 3.ogg");
    }
    m_modeTitleText = "SUDDEN DEATH";
    m_modeDescriptionText = "Tiebreaker round! Don't give up!";
    m_readyBGColor = RGBA::RED;
    m_readyTextColor = RGBA::WHITE;

    HideBackground();
}

//-----------------------------------------------------------------------------------
SuddenDeathMinigameMode::~SuddenDeathMinigameMode()
{

}

//-----------------------------------------------------------------------------------
void SuddenDeathMinigameMode::Initialize(const std::vector<PlayerShip*>& players)
{
    SetBackground("BattleBackground", Vector2(25.0f));
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 10.0f;
    SpriteGameRenderer::instance->SetWorldBounds(AABB2(Vector2(-10.0f), Vector2(10.0f)));
    GameMode::Initialize(players);

    InitializePlayerData();
    //SpawnGeometry(); - This causes some sort of bugs, look into this please ;w;
    SpawnPlayers();
    m_isPlaying = true;
}

//-----------------------------------------------------------------------------------
void SuddenDeathMinigameMode::CleanUp()
{
    GameMode::CleanUp();
}

//-----------------------------------------------------------------------------------
void SuddenDeathMinigameMode::SpawnPlayers()
{
    for (PlayerShip* player : m_players)
    {
        m_entities.push_back(player);
        player->Respawn();
        player->m_scoreText->Enable();
    }
}

//-----------------------------------------------------------------------------------
void SuddenDeathMinigameMode::SpawnGeometry()
{
    //Add in some Asteroids (for color)
    for (int i = 0; i < 20; ++i)
    {
        m_entities.push_back(new Asteroid(GetRandomLocationInArena()));
    }

    //Spawn the all-consuming black-hole
    float radius = 0.5f;
    RemoveEntitiesInCircle(Vector2::ZERO, radius);
    BlackHoleEncounter blackHole(Vector2::ZERO, radius);
    blackHole.Spawn();
    blackHole.m_spawnedBlackHole->m_growsOverTime = true;

}

//-----------------------------------------------------------------------------------
void SuddenDeathMinigameMode::UpdatePlayerScoreDisplay(PlayerShip* player)
{
    int numKills = m_playerStats[player]->m_numKills;
    player->m_scoreText->m_text = Stringf("Kills: %03i", numKills);
}

//-----------------------------------------------------------------------------------
void SuddenDeathMinigameMode::Update(float deltaSeconds)
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

    EndGameIfTooFewPlayers();
    for (PlayerShip* player : m_players)
    {
        UpdatePlayerScoreDisplay(player);
    }

    UpdatePlayerCameras();
}

//-----------------------------------------------------------------------------------
Encounter* SuddenDeathMinigameMode::GetRandomMinorEncounter(const Vector2& center, float radius)
{
    int random = MathUtils::GetRandomIntFromZeroTo(4);
    switch (random)
    {
    case 0:
    case 1:
        return new NebulaEncounter(center, radius);
    case 2:
    case 3:
        return new BossteroidEncounter(center, radius);
    default:
        ERROR_AND_DIE("Random medium encounter roll out of range");
    }
}

//-----------------------------------------------------------------------------------
void SuddenDeathMinigameMode::SetUpPlayerSpawnPoints()
{
    AABB2 bounds = GetArenaBounds();
    AddPlayerSpawnPoint(Vector2::ONE);
    AddPlayerSpawnPoint(Vector2(1.0f, -1.0f));
    AddPlayerSpawnPoint(-Vector2::ONE);
    AddPlayerSpawnPoint(Vector2(-1.0f, 1.0f));
}