#include "Game/GameModes/Minigames/GladiatorMinigameMode.hpp"
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
#include "Game/Encounters/HealingZoneEncounter.hpp"

//-----------------------------------------------------------------------------------
GladiatorMinigameMode::GladiatorMinigameMode()
    : BaseMinigameMode()
{
    m_gameLengthSeconds = 120.0f;
    m_respawnAllowed = true;
    if (!g_disableMusic)
    {
        m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Sweet Tooth - 01 Hard Candy.ogg");
    }
    m_modeTitleText = "Gladiator";
    m_modeDescriptionText = "Only the Gladiator can score kills!";
    m_readyBGColor = RGBA::KHAKI;
    m_readyTextColor = RGBA::CHOCOLATE;

    HideBackground();
}

//-----------------------------------------------------------------------------------
GladiatorMinigameMode::~GladiatorMinigameMode()
{

}

//-----------------------------------------------------------------------------------
void GladiatorMinigameMode::Initialize(const std::vector<PlayerShip*>& players)
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
void GladiatorMinigameMode::InitializePlayerData()
{
    int firstGladiator = MathUtils::GetRandomIntFromZeroTo(m_players.size());
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        PlayerShip* player = m_players[i];
        m_playerStats[player] = new GladiatorStats(player);
        if (firstGladiator == i)
        {
            static_cast<GladiatorStats*>(m_playerStats[player])->m_isGladiator = true;
        }
    }
}

//-----------------------------------------------------------------------------------
void GladiatorMinigameMode::RecordPlayerKill(PlayerShip* killer, Ship* victimShip)
{
    PlayerShip* victim = dynamic_cast<PlayerShip*>(victimShip);
    ASSERT_OR_DIE(victim, "Killed an AI ship during Gladiator (or null target killed), don't spawn those here please >:T");
    GladiatorStats* killerStats = static_cast<GladiatorStats*>(m_playerStats[killer]);
    GladiatorStats* victimStats = static_cast<GladiatorStats*>(m_playerStats[victim]);
    if (killerStats->m_isGladiator)
    {
        killerStats->m_numKills += 1;
    }
    if (victimStats->m_isGladiator)
    {
        killerStats->m_isGladiator = true;
        victimStats->m_isGladiator = false;
    }
}

//-----------------------------------------------------------------------------------
void GladiatorMinigameMode::CleanUp()
{
    GameMode::CleanUp();
}

//-----------------------------------------------------------------------------------
void GladiatorMinigameMode::SpawnPlayers()
{
    for (PlayerShip* player : m_players)
    {
        m_entities.push_back(player);
        player->Respawn();
        player->m_scoreText->Enable();
    }
}

//-----------------------------------------------------------------------------------
void GladiatorMinigameMode::SpawnGeometry()
{
    //Add in some Asteroids (for color)
    for (int i = 0; i < 20; ++i)
    {
        Asteroid* newAsteroid = new Asteroid(GetRandomLocationInArena());
        newAsteroid->m_currentGameMode = this;
        m_entities.push_back(newAsteroid);
    }
}

//-----------------------------------------------------------------------------------
void GladiatorMinigameMode::UpdatePlayerScoreDisplay(PlayerShip* player)
{
    int numKills = m_playerStats[player]->m_numKills;
    player->m_scoreText->m_text = Stringf("Kills: %03i", numKills);
}

//-----------------------------------------------------------------------------------
void GladiatorMinigameMode::Update(float deltaSeconds)
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

        if (static_cast<GladiatorStats*>(m_playerStats[player])->m_isGladiator)
        {
            ParticleSystem::PlayOneShotParticleEffect("Gladiator", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &(player->m_transform));
        }
    }

    UpdatePlayerCameras();
}

//-----------------------------------------------------------------------------------
Encounter* GladiatorMinigameMode::GetRandomMinorEncounter(const Vector2& center, float radius)
{
    int random = MathUtils::GetRandomIntFromZeroTo(4);
    switch (random)
    {
    case 0:
    case 1:
        return new NebulaEncounter(center, radius);
    case 2:
        return new HealingZoneEncounter(center, radius);
    case 3:
        return new BossteroidEncounter(center, radius);
    default:
        ERROR_AND_DIE("Random medium encounter roll out of range");
    }
}

//-----------------------------------------------------------------------------------
void GladiatorMinigameMode::SetUpPlayerSpawnPoints()
{
    //Random Start positions
}