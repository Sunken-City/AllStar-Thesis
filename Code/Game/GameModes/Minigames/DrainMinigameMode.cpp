#include "Game/GameModes/Minigames/DrainMinigameMode.hpp"
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
#include "Game/Encounters/HealingZoneEncounter.hpp"

//-----------------------------------------------------------------------------------
DrainMinigameMode::DrainMinigameMode()
    : BaseMinigameMode()
{
    m_gameLengthSeconds = 120.0f;
    m_respawnAllowed = false;
    if (!g_disableMusic)
    {
        m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Jumper - 08 Moon Machine.ogg");
    }
    m_modeTitleText = "DRAIN";
    m_modeDescriptionText = "Everyone's losing health! Survive!";
    m_readyBGColor = RGBA::GBDARKGREEN;
    m_readyTextColor = RGBA::JOLTIK_PURPLE;

    HideBackground();
}

//-----------------------------------------------------------------------------------
DrainMinigameMode::~DrainMinigameMode()
{

}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::Initialize(const std::vector<PlayerShip*>& players)
{
    SetBackground("BattleBackground", Vector2(25.0f));
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 10.0f;
    SpriteGameRenderer::instance->SetWorldBounds(AABB2(Vector2(-20.0f), Vector2(20.0f)));
    GameMode::Initialize(players);

    InitializePlayerData();
    SpawnGeometry();
    SpawnPlayers();
    SpawnEncounters();
    m_isPlaying = true;
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::CleanUp()
{
    GameMode::CleanUp();
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::SpawnPlayers()
{
    for (PlayerShip* player : m_players)
    {
        m_entities.push_back(player);
        player->Respawn();
        player->m_scoreText->Enable();
    }
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::SpawnGeometry()
{
    //Add in some Asteroids (for color)
    for (int i = 0; i < 20; ++i)
    {
        m_entities.push_back(new Asteroid(GetRandomLocationInArena()));
    }
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::UpdatePlayerScoreDisplay(PlayerShip* player)
{
    int numKills = m_playerStats[player]->m_numKills;
    player->m_scoreText->m_text = Stringf("Kills: %03i", numKills);
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::InitializePlayerData()
{
    for (PlayerShip* player : m_players)
    {
        DrainStats* stats = new DrainStats(player);
        stats->m_timeAlive = 0.0f;
        m_playerStats[player] = stats;
    }
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::RecordPlayerDeath(PlayerShip* ship)
{
    GameMode::RecordPlayerDeath(ship);
    ((DrainStats*)m_playerStats[ship])->m_timeAlive = GetTimerSecondsElapsed();
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::Update(float deltaSeconds)
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
    
    bool shouldDrain = false;
    m_currentDrainInterval = Lerp(GetTimerSecondsElapsed() / m_gameLengthSeconds, DRAIN_INTERVAL_MIN, DRAIN_INTERVAL_MAX);
    m_currentDamagePerInterval = Lerp(GetTimerSecondsElapsed() / m_gameLengthSeconds, DRAIN_DAMAGE_PER_INTERVAL_MIN, DRAIN_DAMAGE_PER_INTERVAL_MAX);

    m_timeSinceLastDrain += deltaSeconds;
    if (m_timeSinceLastDrain >= m_currentDrainInterval)
    {
        shouldDrain = true;
        m_timeSinceLastDrain = 0.0f;
    }

    EndGameIfTooFewPlayers();
    for (PlayerShip* player : m_players)
    {
        UpdatePlayerScoreDisplay(player);
        if (shouldDrain && player->IsAlive())
        {
            player->Drain(m_currentDamagePerInterval);
        }
    }

    UpdatePlayerCameras();
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::RankPlayers()
{
    float* scores = new float[TheGame::instance->m_numberOfPlayers];
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        scores[i] = INT_MIN;
        PlayerShip* ship = m_players[i];
        DrainStats* stats = (DrainStats*)m_playerStats[ship];
        float timeAliveBonus = stats->m_timeAlive == 0.0f ? 10000.0f : stats->m_timeAlive;
        scores[i] = (float)stats->m_numKills + timeAliveBonus;
        ship->m_rank = 999;
    }
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        int numBetterPlayers = 0;
        int myScore = scores[i];
        for (unsigned int j = 0; j < m_players.size(); ++j)
        {
            int otherScore = scores[j];
            if (myScore < otherScore)
            {
                ++numBetterPlayers;
            }
        }
        m_players[i]->m_rank = numBetterPlayers + 1; //1st place has 0 people better
    }
    delete scores;
}

//-----------------------------------------------------------------------------------
Encounter* DrainMinigameMode::GetRandomMinorEncounter(const Vector2& center, float radius)
{
    int random = MathUtils::GetRandomIntFromZeroTo(7);
    switch (random)
    {
    case 0:
        return new NebulaEncounter(center, radius);
    case 1:
        return new BossteroidEncounter(center, radius);
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        return new HealingZoneEncounter(center, radius);
    default:
        ERROR_AND_DIE("Random medium encounter roll out of range");
    }
}

//-----------------------------------------------------------------------------------
void DrainMinigameMode::SetUpPlayerSpawnPoints()
{
    //Random start positions
}