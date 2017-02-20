#include "Game/GameModes/GameMode.hpp"
#include "Game/TheGame.hpp"
#include "Game/StateMachine.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"
#include "Game/Entities/Pickup.hpp"
#include "Game/Entities/Ship.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/UI/UISystem.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Renderer/2D/TextRenderable2D.hpp"
#include "../Entities/TextSplash.hpp"
#include "../Encounters/SquadronEncounter.hpp"
#include "../Encounters/NebulaEncounter.hpp"
#include "../Encounters/WormholeEncounter.hpp"
#include "../Encounters/BlackHoleEncounter.hpp"

//-----------------------------------------------------------------------------------
GameMode::GameMode(const std::string& arenaBackgroundImage)
    : m_arenaBackground(new Sprite(arenaBackgroundImage, TheGame::BACKGROUND_LAYER))
    , m_starfield(new Sprite("Starfield", TheGame::BACKGROUND_STARS_LAYER))
    , m_starfield2(new Sprite("Starfield", TheGame::BACKGROUND_STARS_LAYER_SLOWER))
    , m_modeTitleText("MODE NAME")
{
    m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/SFX/Music/PlaceholderMusic1.m4a");
    
    m_starfield->m_transform.SetScale(Vector2(5.0f));
    m_starfield2->m_transform.SetScale(Vector2(16.0f));
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_STARS_LAYER)->m_virtualScaleMultiplier = 0.98f;
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_STARS_LAYER_SLOWER)->m_virtualScaleMultiplier = 2.0f;

    InitializePlayerData();
}

//-----------------------------------------------------------------------------------
GameMode::~GameMode()
{
    StopPlaying();

    for (auto iterator : m_playerStats)
    {
        delete iterator.second;
    }
    delete m_arenaBackground;
    m_arenaBackground = nullptr;
    delete m_starfield;
    m_starfield = nullptr;
    delete m_starfield2;
    m_starfield2 = nullptr;
    UISystem::instance->DeleteWidget(m_timerWidget);
    m_timerWidget = nullptr;
}

//-----------------------------------------------------------------------------------
void GameMode::Initialize()
{
    if (!m_muteMusic)
    {
        AudioSystem::instance->PlayLoopingSound(m_backgroundMusic, 0.6f);
    }

    ShowBackground();

    m_timerWidget = UISystem::instance->CreateWidget("Label");
    m_timerWidget->SetProperty<std::string>("Name", "GameTimer");
    m_timerWidget->SetProperty<std::string>("Text", "00:00");
    m_timerWidget->SetProperty("BackgroundColor", RGBA::CLEAR);
    m_timerWidget->SetProperty("BorderWidth", 0.0f);
    m_timerWidget->SetProperty("TextSize", 4.0f);
    m_timerWidget->SetProperty("Offset", Vector2(675.0f, 750.0f));
    UISystem::instance->AddWidget(m_timerWidget);
}

//-----------------------------------------------------------------------------------
void GameMode::CleanUp()
{
    StopPlaying();
    AudioSystem::instance->StopSound(m_backgroundMusic);
}

//-----------------------------------------------------------------------------------
void GameMode::Update(float deltaSeconds)
{
    if (m_isPlaying)
    {
        m_timerSecondsElapsed += deltaSeconds;
        int timeRemainingSeconds = static_cast<int>(m_gameLengthSeconds - m_timerSecondsElapsed);
        m_timerWidget->SetProperty<std::string>("Text", Stringf("%02i:%02i", timeRemainingSeconds / 60, timeRemainingSeconds % 60));
    }
    if (m_timerSecondsElapsed >= m_gameLengthSeconds)
    {
        StopPlaying();
    }
}

//-----------------------------------------------------------------------------------
Vector2 GameMode::GetRandomLocationInArena(float radius /*= 0.0f*/)
{
    return AABB2::CreateMinkowskiBox(GetArenaBounds(), -radius).GetRandomPointInside();
}

//-----------------------------------------------------------------------------------
Vector2 GameMode::GetRandomPlayerSpawnPoint()
{
    if (m_playerSpawnPoints.size() > 0)
    {
        int randomPoint = MathUtils::GetRandomIntFromZeroTo(m_playerSpawnPoints.size());
        return m_playerSpawnPoints[randomPoint];
    }
    else
    {
        return GetRandomLocationInArena();
    }
}

//-----------------------------------------------------------------------------------
void GameMode::RemoveEntitiesInCircle(const Vector2& center, float radius)
{
    const float radiusSquared = radius * radius;

    for (auto iter = m_entities.begin(); iter != m_entities.end();)
    {
        Entity* entity = *iter;
        float distSquared = MathUtils::CalcDistSquaredBetweenPoints(entity->m_transform.GetWorldPosition(), center);
        float combinedRadius = entity->m_collisionRadius + radius;
        float combinedRadiusSquared = combinedRadius * combinedRadius;
        bool isInRadius = distSquared < combinedRadiusSquared;
        //Debug distances code:
        //TextSplash::CreateTextSplash(Stringf("%i : %s", static_cast<int>(distSquared), isInRadius ? "True" : "False"), entity->m_transform.GetWorldPosition(), Vector2::ZERO, RGBA::GBLIGHTGREEN);
        
        if (!entity->IsPlayer() && isInRadius)
        {
            delete entity;
            iter = m_entities.erase(iter);
            continue;
        }
        if (iter == m_entities.end())
        {
            break;
        }
        ++iter;
    }
}

//-----------------------------------------------------------------------------------
void GameMode::AddPlayerSpawnPoint(const Vector2& newSpawnPoint)
{
    m_playerSpawnPoints.push_back(newSpawnPoint);
}

//-----------------------------------------------------------------------------------
AABB2 GameMode::GetArenaBounds()
{
   return SpriteGameRenderer::instance->m_worldBounds;
}

//-----------------------------------------------------------------------------------
void GameMode::SpawnBullet(Projectile*bullet)
{
    m_newEntities.push_back(bullet);
}

//-----------------------------------------------------------------------------------
void GameMode::SpawnPickup(Item* item, const Vector2& spawnPosition)
{
    ASSERT_OR_DIE(item, "Item was null when attempting to spawn pickup");
    m_newEntities.push_back(new Pickup(item, spawnPosition));
}

//-----------------------------------------------------------------------------------
void GameMode::SetBackground(const std::string& backgroundName, const Vector2& scale)
{
    if (m_arenaBackground)
    {
        delete m_arenaBackground;
        m_arenaBackground = nullptr;
    }
    m_arenaBackground = new Sprite(backgroundName, TheGame::BACKGROUND_LAYER);
    m_arenaBackground->m_transform.SetScale(scale);
    m_arenaBackground->m_tintColor = RGBA::GRAY;
    m_arenaBackground->Enable();
}

//-----------------------------------------------------------------------------------
float GameMode::CalculateAttenuation(const Vector2& soundPosition)
{
    float attenuationVolume = 0.0f;
    for (PlayerShip* player : TheGame::instance->m_players)
    {
        float distance = MathUtils::CalcDistSquaredBetweenPoints(player->GetPosition(), soundPosition);
        float currentAttenuationVolume = 1.0f - (distance / 100.0f);
        attenuationVolume = Max(attenuationVolume, currentAttenuationVolume);
    }
    return attenuationVolume;
}

//-----------------------------------------------------------------------------------
void GameMode::PlaySoundAt(const SoundID sound, const Vector2& soundPosition, float maxVolume)
{
    float attenuationVolume = CalculateAttenuation(soundPosition);
    float clampedVolume = Min<float>(attenuationVolume, maxVolume);
    AudioSystem::instance->PlaySound(sound, clampedVolume);
}

//-----------------------------------------------------------------------------------
GameMode* GameMode::GetCurrent()
{
    return TheGame::instance->m_currentGameMode;
}

//-----------------------------------------------------------------------------------
void GameMode::StopPlaying()
{
    m_isPlaying = false;
    if (m_timerWidget)
    {
        m_timerWidget->SetHidden();
    }
}

//-----------------------------------------------------------------------------------
void GameMode::HideBackground()
{
    m_arenaBackground->Disable();
    m_starfield->Disable();
    m_starfield2->Disable();
}

//-----------------------------------------------------------------------------------
void GameMode::ShowBackground()
{
    m_arenaBackground->Enable();
    m_starfield->Enable();
    m_starfield2->Enable();
}

//-----------------------------------------------------------------------------------
void GameMode::InitializeReadyAnim()
{
    m_rotationTime = 0.0f;
    m_leftSpindleCenter.SetPosition(Vector2(-16.0f, 5.5f));
    m_rightSpindleCenter.SetPosition(Vector2(12.0f, -3.0f));
    for (int i = 0; i < 3; ++i)
    {
        Sprite* leftSpindle = new Sprite("SpindleArm", TheGame::UI_LAYER);
        m_leftSpindleCenter.AddChild(&leftSpindle->m_transform);
        leftSpindle->m_transform.SetScale(Vector2(7.0f));
        leftSpindle->m_transform.SetRotationDegrees((i + 1) * 90.0f);
        leftSpindle->m_tintColor = RGBA::BLACK;
        m_leftSpindles[i] = leftSpindle;

        Sprite* rightSpindle = new Sprite("SpindleArm", TheGame::UI_LAYER);
        m_rightSpindleCenter.AddChild(&rightSpindle->m_transform);
        rightSpindle->m_transform.SetScale(Vector2(5.1f));
        rightSpindle->m_transform.SetRotationDegrees((i)* 90.0f);
        rightSpindle->m_tintColor = RGBA::BLACK;
        m_rightSpindles[i] = rightSpindle;
    }

    m_modeTitleRenderable = new TextRenderable2D(m_modeTitleText, Transform2D(Vector2(0.0f, 1.0f)), TheGame::TEXT_LAYER, true);
    m_getReadyRenderable = new TextRenderable2D("Get Ready!", Transform2D(Vector2(0.0f, -3.0f)), TheGame::TEXT_LAYER, true);
    m_modeTitleRenderable->m_color = RGBA::RED;
    m_getReadyRenderable->m_color = RGBA::RED;
    m_modeTitleRenderable->m_fontSize = 1.2f;
    m_modeTitleRenderable->Disable();
    m_getReadyRenderable->Disable();
    HideBackground();
}

//-----------------------------------------------------------------------------------
void GameMode::UpdateReadyAnim(float deltaSeconds)
{
    m_rotationTime = Lerp<float>(0.0f, 1.0f, Clamp<float>(g_secondsInState * 2.0f, 0.0f, 1.0f));
    float rotationTime2 = Lerp<float>(0.0f, 1.0f, Clamp<float>((g_secondsInState * 2.0f) - 0.5f, 0.0f, 1.0f));
    m_leftSpindleCenter.SetRotationDegrees(45.0f + (45.0f * m_rotationTime));
    m_rightSpindleCenter.SetRotationDegrees(-90.0f + (75.0f * rotationTime2));

    if (m_rotationTime == 1.0f)
    {
        m_modeTitleRenderable->Enable();
    }
    if (rotationTime2 == 1.0f)
    {
        m_getReadyRenderable->Enable();
    }
}

//-----------------------------------------------------------------------------------
void GameMode::CleanupReadyAnim()
{
    for (int i = 0; i < 3; ++i)
    {
        delete m_leftSpindles[i];
        delete m_rightSpindles[i];
    }
    delete m_modeTitleRenderable;
    delete m_getReadyRenderable;
}

//-----------------------------------------------------------------------------------
void GameMode::InitializePlayerData()
{
    for (PlayerShip* player : TheGame::instance->m_players)
    {
        m_playerStats[player] = new DefaultPlayerStats(player);
    }
}

//-----------------------------------------------------------------------------------
void GameMode::RecordPlayerDeath(PlayerShip* ship)
{
    DefaultPlayerStats* stats = m_playerStats[ship];
    stats->m_numDeaths += 1;
}

//-----------------------------------------------------------------------------------
void GameMode::RecordPlayerKill(PlayerShip* killer, Ship*)
{
    DefaultPlayerStats* stats = m_playerStats[killer];
    stats->m_numKills += 1;
}

//-----------------------------------------------------------------------------------
void GameMode::DetermineWinners()
{
    int maxScore = 0;
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        DefaultPlayerStats* stats = m_playerStats[ship];
        int playerScore = stats->m_numKills - stats->m_numDeaths;
        maxScore = playerScore > maxScore ? playerScore : maxScore;
    }
    for (PlayerShip* ship : TheGame::instance->m_players)
    {
        DefaultPlayerStats* stats = m_playerStats[ship];
        int playerScore = stats->m_numKills - stats->m_numDeaths;
        if (playerScore == maxScore)
        {
            ship->m_sprite->m_tintColor = RGBA::GOLD;
        }
        else
        {
            ship->m_sprite->m_tintColor = RGBA::GRAY;
        }
    }
}

//-----------------------------------------------------------------------------------
Vector2 GameMode::FindSpaceForEncounter(float radius, const std::vector<Encounter*>& encounters)
{
    Vector2 location;
    bool foundLocation = false;
    int iterationCount = 0;

    do
    {
        location = GetRandomLocationInArena(radius);
        foundLocation = true;

        if (encounters.size() == 0)
        {
            return location;
        }
        for (Encounter* encounter : encounters)
        {
            float combinedRadius = radius + encounter->m_radius;
            float combinedDistanceSquared = combinedRadius * combinedRadius;
            if (MathUtils::CalcDistSquaredBetweenPoints(encounter->m_center, location) <= combinedDistanceSquared)
            {
                foundLocation = false;
                break;
            }
        }

        ASSERT_OR_DIE(++iterationCount < 1000, "Ran out of space for an encounter, is your map too small or the encounter too big?");
    } while (!foundLocation);
    return location;
}

//-----------------------------------------------------------------------------------
Encounter* GameMode::GetRandomMediumEncounter(const Vector2& center, float radius)
{
    int random = MathUtils::GetRandomIntFromZeroTo(2);
    switch (random)
    {
    case 0:
        return new NebulaEncounter(center, radius);
    case 1:
        return new NebulaEncounter(center, radius);
    default:
        ERROR_AND_DIE("Random medium encounter roll out of range");
    }
}

//-----------------------------------------------------------------------------------
Encounter* GameMode::GetRandomLargeEncounter(const Vector2& center, float radius)
{
    int random = MathUtils::GetRandomIntFromZeroTo(2);
    switch (random)
    {
    case 0:
        return new WormholeEncounter(center, radius);
    case 1:
        return new BlackHoleEncounter(center, radius);
    default:
        ERROR_AND_DIE("Random medium encounter roll out of range");
    }
}