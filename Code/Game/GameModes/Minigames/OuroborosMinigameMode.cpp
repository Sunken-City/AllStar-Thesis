#include "Game/GameModes/Minigames/OuroborosMinigameMode.hpp"
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
#include "Game/Entities/MinigameEntities/Coin.hpp"
#include "Game/Entities/MinigameEntities/OuroborosCoin.hpp"

//-----------------------------------------------------------------------------------
OuroborosMinigameMode::OuroborosMinigameMode()
    : BaseMinigameMode()
{
    m_gameLengthSeconds = 120.0f;
    m_respawnAllowed = true;
    if (!g_disableMusic)
    {
        m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/Music/Foxx - Sweet Tooth - 03 Sorbet.ogg");
    }
    m_modeTitleText = "OUROBOROS";
    m_modeDescriptionText = "Grab coins dropped by ships!";
    m_readyBGColor = RGBA::CHOCOLATE;
    m_readyTextColor = RGBA::YELLOW;

    HideBackground();
}

//-----------------------------------------------------------------------------------
OuroborosMinigameMode::~OuroborosMinigameMode()
{

}

//-----------------------------------------------------------------------------------
void OuroborosMinigameMode::Initialize(const std::vector<PlayerShip*>& players)
{
    SetBackground("RaceBackground", Vector2(25.0f));
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
void OuroborosMinigameMode::CleanUp()
{
    GameMode::CleanUp();
}

//-----------------------------------------------------------------------------------
void OuroborosMinigameMode::SpawnPlayers()
{
    for (PlayerShip* player : m_players)
    {
        m_entities.push_back(player);
        player->Respawn();
        player->m_scoreText->Enable();
    }
}

//-----------------------------------------------------------------------------------
void OuroborosMinigameMode::SpawnGeometry()
{
    //Add in some Asteroids (for color)
    for (int i = 0; i < 20; ++i)
    {
        Asteroid* newAsteroid = new Asteroid(GetRandomLocationInArena());
        newAsteroid->m_currentGameMode = this;
        m_entities.push_back(newAsteroid);
    }

    for (int i = 0; i < 20; ++i)
    {
        Coin* newCoin = new Coin(GetRandomLocationInArena());
        newCoin->m_currentGameMode = this;
        m_entities.push_back(newCoin);
    }
}

//-----------------------------------------------------------------------------------
void OuroborosMinigameMode::UpdatePlayerScoreDisplay(PlayerShip* player)
{
    OuroborosPlayerStats* playerStats = (OuroborosPlayerStats*)m_playerStats[player];
    int numCoins = playerStats->m_numberOfCoins;
    player->m_scoreText->m_text = Stringf("Coins: %03i", numCoins);
}

//-----------------------------------------------------------------------------------
void OuroborosMinigameMode::RecordPlayerDeath(PlayerShip* ship)
{
    GameMode::RecordPlayerDeath(ship);

    //Lose 10% of your coins for dying. Sorry fam.
    OuroborosPlayerStats* playerInfo = (OuroborosPlayerStats*)m_playerStats[ship];
    int numCoinsDropped = (int)floor((float)playerInfo->m_numberOfCoins * 0.1f);
    playerInfo->m_numberOfCoins -= numCoinsDropped;

    Vector2 dropPosition = ship->m_transform.GetWorldPosition();
    while (numCoinsDropped != 0)
    {
        if (numCoinsDropped - Coin::GOLD_VALUE >= 0)
        {
            Coin* newCoin = new Coin(dropPosition, Coin::GOLD_VALUE);
            newCoin->m_currentGameMode = this;
            m_newEntities.push_back(newCoin);
            numCoinsDropped -= Coin::GOLD_VALUE;
        }
        else if (numCoinsDropped - Coin::SILVER_VALUE >= 0)
        {
            Coin* newCoin = new Coin(dropPosition, Coin::SILVER_VALUE);
            newCoin->m_currentGameMode = this;
            m_newEntities.push_back(newCoin);
            numCoinsDropped -= Coin::SILVER_VALUE;
        }
        else
        {
            Coin* newCoin = new Coin(dropPosition, Coin::BRONZE_VALUE);
            newCoin->m_currentGameMode = this;
            m_newEntities.push_back(newCoin);
            numCoinsDropped -= Coin::BRONZE_VALUE;
        }
    }
}

//-----------------------------------------------------------------------------------
void OuroborosMinigameMode::RecordPlayerPickupCoin(PlayerShip* ship, int coinValue)
{
    OuroborosPlayerStats* playerInfo = (OuroborosPlayerStats*)m_playerStats[ship];
    playerInfo->m_numberOfCoins += coinValue;
}

//-----------------------------------------------------------------------------------
void OuroborosMinigameMode::InitializePlayerData()
{
    for (PlayerShip* player : m_players)
    {
        m_playerStats[player] = new OuroborosPlayerStats(player);
    }
}

//-----------------------------------------------------------------------------------
void OuroborosMinigameMode::RankPlayers()
{
    int* scores = new int[TheGame::instance->m_numberOfPlayers];
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        scores[i] = INT_MIN;
        PlayerShip* ship = m_players[i];
        OuroborosPlayerStats* playerStats = (OuroborosPlayerStats*)m_playerStats[ship];
        scores[i] = playerStats->m_numberOfCoins;
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
void OuroborosMinigameMode::Update(float deltaSeconds)
{
    BaseMinigameMode::Update(deltaSeconds);
    deltaSeconds = m_scaledDeltaSeconds;

    if (!m_isPlaying)
    {
        return;
    }

    m_timeSinceLastCoin += deltaSeconds;
    if (m_timeSinceLastCoin > SECONDS_BETWEEN_COIN_SPAWNS)
    {
        //Ships all drop coins that they can't pick up.
        for (PlayerShip* ship : m_players)
        {
            OuroborosCoin* ouroborosCoin = new OuroborosCoin(ship, ship->m_transform.GetWorldPosition());
            ouroborosCoin->m_currentGameMode = this;
            m_newEntities.push_back(ouroborosCoin);
        }
        Coin* newCoin = new Coin(GetRandomLocationInArena());
        newCoin->m_currentGameMode = this;
        m_newEntities.push_back(newCoin);

        m_timeSinceLastCoin = 0.0f;
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
Encounter* OuroborosMinigameMode::GetRandomMinorEncounter(const Vector2& center, float radius)
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
void OuroborosMinigameMode::SetUpPlayerSpawnPoints()
{
    //Empty, use the random spawn position list.
}