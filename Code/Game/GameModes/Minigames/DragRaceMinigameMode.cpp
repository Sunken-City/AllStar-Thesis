#include "Game/GameModes/Minigames/DragRaceMinigameMode.hpp"
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
#include "Engine/Renderer/2D/BarGraphRenderable2D.hpp"

//-----------------------------------------------------------------------------------
DragRaceMinigameMode::DragRaceMinigameMode()
    : BaseMinigameMode()
    , m_winZone(Vector2(-HALF_ARENA_WIDTH, HALF_ARENA_HEIGHT - FINISH_LINE_HEIGHT), Vector2(HALF_ARENA_WIDTH, HALF_ARENA_HEIGHT))
{
    m_gameLengthSeconds = 120.0f;
    m_respawnAllowed = true;
    if (!g_disableMusic)
    {
        m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Sweet Tooth - 04 Strawberry.ogg");
    }
    m_modeTitleText = "DRAG RACE";
    m_modeDescriptionText = "It's a race to the finish!";
    m_readyBGColor = RGBA(0xA840A8FF);
    m_readyTextColor = RGBA(0xF88000FF);

    m_winZoneRenderer = new BarGraphRenderable2D(m_winZone, RGBA::GREEN, RGBA::CLEAR, TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER);
    m_winZoneRenderer->SetPercentageFilled(1.0f);

    HideBackground();
}

//-----------------------------------------------------------------------------------
DragRaceMinigameMode::~DragRaceMinigameMode()
{
    delete m_winZoneRenderer;
}

//-----------------------------------------------------------------------------------
void DragRaceMinigameMode::Initialize(const std::vector<PlayerShip*>& players)
{
    SetBackground("RaceBackground", Vector2(25.0f));
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_LAYER)->m_virtualScaleMultiplier = 10.0f;
    SpriteGameRenderer::instance->SetWorldBounds(AABB2(Vector2(-HALF_ARENA_WIDTH, -HALF_ARENA_HEIGHT), Vector2(HALF_ARENA_WIDTH, HALF_ARENA_HEIGHT)));
    GameMode::Initialize(players);

    InitializePlayerData();
    SpawnGeometry();
    SpawnEncounters();
    SpawnPlayers();
    m_isPlaying = true;
}

//-----------------------------------------------------------------------------------
void DragRaceMinigameMode::CleanUp()
{
    GameMode::CleanUp();
}

//-----------------------------------------------------------------------------------
void DragRaceMinigameMode::SpawnPlayers()
{
    for (PlayerShip* player : m_players)
    {
        m_entities.push_back(player);
        player->Respawn();
        player->m_scoreText->Enable();
    }
}

//-----------------------------------------------------------------------------------
void DragRaceMinigameMode::SpawnGeometry()
{
    //Add in some Asteroids (for color)
    for (int i = 0; i < 30; ++i)
    {
        Asteroid* newAsteroid = new Asteroid(GetRandomLocationInArena());
        newAsteroid->m_currentGameMode = this;
        m_entities.push_back(newAsteroid);
    }
}

//-----------------------------------------------------------------------------------
void DragRaceMinigameMode::UpdatePlayerScoreDisplay(PlayerShip* player)
{
    static const float finishLineY = HALF_ARENA_HEIGHT - FINISH_LINE_HEIGHT;
    float playerY = player->m_transform.GetWorldPosition().y;
    float distanceToFinish = finishLineY - playerY;

    if (distanceToFinish < 0.0f)
    {
        distanceToFinish = 0.0f;
        if (((RacePlayerStats*)m_playerStats[player])->m_timeToFinish == m_gameLengthSeconds)
        {
            ((RacePlayerStats*)m_playerStats[player])->m_timeToFinish = GetTimerRealSecondsElapsed();
            if (GetTimerSecondsElapsed() < m_gameLengthSeconds)
            {
                SetTimeRemaining(0.0f);
            }
        }
    }
    if (((RacePlayerStats*)m_playerStats[player])->m_timeToFinish != m_gameLengthSeconds)
    {
        ((RacePlayerStats*)m_playerStats[player])->m_distanceToFinish = 0.0f;
    }
    else
    {
        ((RacePlayerStats*)m_playerStats[player])->m_distanceToFinish = distanceToFinish;
    }
    player->m_scoreText->m_text = Stringf("Dist Left: %03.1f", distanceToFinish);
}

//-----------------------------------------------------------------------------------
void DragRaceMinigameMode::InitializePlayerData()
{
    for (PlayerShip* player : m_players)
    {
        RacePlayerStats* stats = new RacePlayerStats(player);
        stats->m_timeToFinish = m_gameLengthSeconds;
        m_playerStats[player] = stats;
    }
}

//-----------------------------------------------------------------------------------
void DragRaceMinigameMode::RankPlayers()
{
    float* scores = new float[TheGame::instance->m_numberOfPlayers];
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        scores[i] = m_gameLengthSeconds + ARENA_HEIGHT;
        PlayerShip* ship = m_players[i];
        RacePlayerStats* stats = (RacePlayerStats*)m_playerStats[ship];
        scores[i] = stats->m_timeToFinish + stats->m_distanceToFinish;
        ship->m_rank = 999;
    }
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        int numBetterPlayers = 0;
        float myTime = scores[i];
        for (unsigned int j = 0; j < m_players.size(); ++j)
        {
            float otherTime = scores[j];
            if (myTime > otherTime)
            {
                ++numBetterPlayers;
            }
        }
        m_players[i]->m_rank = numBetterPlayers + 1; //1st place has 0 people better
    }
    delete scores;
}

//-----------------------------------------------------------------------------------
void DragRaceMinigameMode::Update(float deltaSeconds)
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

    for (PlayerShip* player : m_players)
    {
        UpdatePlayerScoreDisplay(player);
    }

    UpdatePlayerCameras();
}

//-----------------------------------------------------------------------------------
Encounter* DragRaceMinigameMode::GetRandomMinorEncounter(const Vector2& center, float radius)
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
void DragRaceMinigameMode::SetUpPlayerSpawnPoints()
{
    m_uniquePlayerSpawns = true;
    AABB2 bounds = GetArenaBounds();
    AddPlayerSpawnPoint(bounds.mins + Vector2(ARENA_WIDTH * 0.2f, 1.0f - HALF_ARENA_HEIGHT));
    AddPlayerSpawnPoint(bounds.mins + Vector2(ARENA_WIDTH * 0.4f, 1.0f - HALF_ARENA_HEIGHT));
    AddPlayerSpawnPoint(bounds.mins + Vector2(ARENA_WIDTH * 0.6f, 1.0f - HALF_ARENA_HEIGHT));
    AddPlayerSpawnPoint(bounds.mins + Vector2(ARENA_WIDTH * 0.8f, 1.0f - HALF_ARENA_HEIGHT));
}