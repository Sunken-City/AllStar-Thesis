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
#include "Game/Pilots/PlayerPilot.hpp"
#include "Game/Entities/TextSplash.hpp"
#include "Game/Encounters/SquadronEncounter.hpp"
#include "Game/Encounters/NebulaEncounter.hpp"
#include "Game/Encounters/WormholeEncounter.hpp"
#include "Game/Entities/Props/BlackHole.hpp"
#include "Game/Encounters/BlackHoleEncounter.hpp"
#include "Game/Encounters/CargoShipEncounter.hpp"
#include "Game/Encounters/BossteroidEncounter.hpp"
#include "Engine/Input/XInputController.hpp"

const double GameMode::AFTER_GAME_SLOWDOWN_SECONDS = 3.0;
const double GameMode::ANIMATION_LENGTH_SECONDS = 1.0;
int GameMode::s_currentVortexId = 0;

//-----------------------------------------------------------------------------------
GameMode::GameMode(const std::string& arenaBackgroundImage)
    : m_arenaBackground(new Sprite(arenaBackgroundImage, TheGame::BACKGROUND_LAYER))
    , m_starfield(new Sprite("Starfield", TheGame::BACKGROUND_STARS_LAYER))
    , m_starfield2(new Sprite("Starfield", TheGame::BACKGROUND_STARS_LAYER_SLOWER))
    , m_modeTitleText("MODE NAME")
    , m_modeDescriptionText("MODE DESCRIPTION")
{
    //m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/SFX/Music/PlaceholderMusic1.m4a");
    
    m_starfield->m_transform.SetScale(Vector2(5.0f));
    m_starfield2->m_transform.SetScale(Vector2(16.0f));
    m_starfield->m_tintColor = RGBA::KINDA_GRAY;
    m_starfield2->m_tintColor = RGBA::KINDA_GRAY;
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_STARS_LAYER)->m_virtualScaleMultiplier = 0.98f;
    SpriteGameRenderer::instance->CreateOrGetLayer(TheGame::BACKGROUND_STARS_LAYER_SLOWER)->m_virtualScaleMultiplier = 2.0f;
}

//-----------------------------------------------------------------------------------
GameMode::~GameMode()
{
    StopPlaying();

    for (auto iterator : m_playerStats)
    {
        delete iterator.second;
    }
    m_playerStats.clear();



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
void GameMode::Initialize(const std::vector<PlayerShip*>& players)
{
    for (PlayerShip* player : players)
    {
        player->m_currentGameMode = this;
        m_players.push_back(player);
    }

    ShowBackground();

    m_countdownWidget = UISystem::instance->CreateWidget("Label");
    m_countdownWidget->SetProperty<std::string>("Name", "Countdown");
    m_countdownWidget->SetProperty<std::string>("Text", "0");
    m_countdownWidget->SetProperty("TextColor", RGBA::RED);
    m_countdownWidget->SetProperty("BackgroundColor", RGBA::CLEAR);
    m_countdownWidget->SetProperty("BorderWidth", 0.0f);
    m_countdownWidget->SetProperty("TextSize", 4.0f);
    m_countdownWidget->SetProperty("Offset", Vector2(800.0f, 375.0f));
    m_countdownWidget->SetProperty<std::string>("Text", "6");
    UISystem::instance->AddWidget(m_countdownWidget);
    m_countdownWidget->SetHidden();

    m_timerWidget = UISystem::instance->CreateWidget("Label");
    m_timerWidget->SetProperty<std::string>("Name", "GameTimer");
    m_timerWidget->SetProperty<std::string>("Text", "00:00");
    m_timerWidget->SetProperty("BackgroundColor", RGBA::CLEAR);
    m_timerWidget->SetProperty("BorderWidth", 0.0f);
    m_timerWidget->SetProperty("TextSize", 4.0f);
    m_timerWidget->SetProperty("Offset", Vector2(675.0f, 775.0f));
    UISystem::instance->AddWidget(m_timerWidget);
}

//-----------------------------------------------------------------------------------
void GameMode::CleanUp()
{
    StopPlaying();
    ClearVortexPositions(); 
    DeleteAllEntities();

    AudioSystem::instance->StopSound(m_backgroundMusic);
}

//-----------------------------------------------------------------------------------
void GameMode::DeleteAllEntities()
{
    for (Entity* ent : m_entities)
    {
        if (!ent->IsPlayer())
        {
            delete ent;
        }
    }
    m_entities.clear();
}

//-----------------------------------------------------------------------------------
void GameMode::Update(float deltaSeconds)
{
    m_scaledDeltaSeconds = deltaSeconds;
    if (m_isPlaying)
    {
        m_timerSecondsElapsed += deltaSeconds;
        int timeRemainingSeconds = static_cast<int>(ceil(m_gameLengthSeconds - m_timerSecondsElapsed));
        int minutesRemaining = timeRemainingSeconds / 60;
        int secondsRemaining = timeRemainingSeconds % 60;
        if (minutesRemaining > 0)
        {
            m_timerWidget->SetProperty<std::string>("Text", Stringf("%01i:%02i", minutesRemaining, secondsRemaining));
        }
        else
        {
            m_timerWidget->SetProperty<std::string>("Text", Stringf(":%02i", secondsRemaining));
        }
    }

    if ((m_timerSecondsElapsed >= (m_gameLengthSeconds - 5.0f)) && m_timerSecondsElapsed < m_gameLengthSeconds)
    {
        static SoundID countdownSounds[5];
        countdownSounds[0] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_1.ogg");
        countdownSounds[1] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_2.ogg");
        countdownSounds[2] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_3.ogg");
        countdownSounds[3] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_4.ogg");
        countdownSounds[4] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/count_5.ogg");

        m_countdownWidget->SetVisible();
        int countdownNumber = stoi(m_countdownWidget->GetProperty<std::string>("Text"));
        int timeRemainingSeconds = static_cast<int>(m_gameLengthSeconds - m_timerSecondsElapsed + 1) % 60;

        m_countdownWidget->SetProperty<std::string>("Text", Stringf("%i", timeRemainingSeconds));
        m_countdownWidget->SetProperty("TextSize", 4.0f + (3.0f / (float)(timeRemainingSeconds + 1)));
        
        if (countdownNumber != timeRemainingSeconds)
        {
            AudioSystem::instance->PlaySound(countdownSounds[timeRemainingSeconds - 1]);
        }
    }
    if (m_timerSecondsElapsed >= m_gameLengthSeconds)
    {
        if (m_countdownWidget->GetProperty<std::string>("Text") != "TIME!")
        {
            AudioSystem::instance->PlaySound(AudioSystem::instance->CreateOrGetSound("Data/SFX/Countdown/time_up.ogg"));
        }
        m_timerWidget->SetProperty<std::string>("Text", ":00");
        m_countdownWidget->SetProperty<std::string>("Text", "TIME!");
        m_countdownWidget->SetProperty("TextSize", 4.0f);
        m_countdownWidget->SetProperty("Offset", Vector2(675.0f, 375.0f));

        double overtimeSeconds = m_timerSecondsElapsed - m_gameLengthSeconds;
        double ratioIntoOvertime = overtimeSeconds / AFTER_GAME_SLOWDOWN_SECONDS;
        m_scaledDeltaSeconds = deltaSeconds * MathUtils::Clamp(MathUtils::SmoothStart2(1.0f - (float)ratioIntoOvertime), 0.2f, 1.0f);
    }
    if (m_timerSecondsElapsed >= m_gameLengthSeconds + AFTER_GAME_SLOWDOWN_SECONDS)
    {
        StopPlaying();
    }
}

//-----------------------------------------------------------------------------------
void GameMode::UpdatePlayerCameras()
{
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        PlayerShip* player = m_players[i];
        Vector2 targetCameraPosition = player->GetPosition();
        Vector2 playerRightStick = player->m_pilot->m_inputMap.GetVector2("ShootRight", "ShootUp");

        if (InputSystem::instance->WasKeyJustPressed('R'))
        {
            float radius = 1.0f;
            RemoveEntitiesInCircle(player->m_transform.GetWorldPosition(), radius);
            BlackHoleEncounter nebby(player->m_transform.GetWorldPosition(), radius);
            nebby.Spawn();
            nebby.m_spawnedBlackHole->m_growsOverTime = true;
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

//-----------------------------------------------------------------------------------
void GameMode::SetTimeRemaining(float timeRemainingSeconds)
{
    m_timerSecondsElapsed = m_gameLengthSeconds - timeRemainingSeconds;
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
void GameMode::SpawnEntityInGameWorld(Entity* entity)
{
    entity->m_currentGameMode = this; 
    m_newEntities.push_back(entity);
}

//-----------------------------------------------------------------------------------
void GameMode::SpawnBullet(Projectile* bullet)
{
    bullet->m_currentGameMode = this;
    m_newEntities.push_back(bullet);
}

//-----------------------------------------------------------------------------------
void GameMode::SpawnPickup(Item* item, const Vector2& spawnPosition)
{
    ASSERT_OR_DIE(item, "Item was null when attempting to spawn pickup");
    Pickup* pickup = new Pickup(item, spawnPosition);
    pickup->m_currentGameMode = this;
    m_newEntities.push_back(pickup);
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
    for (PlayerShip* player : m_players)
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
    if (m_countdownWidget)
    {
        m_countdownWidget->SetHidden();
    }
    if (m_timerWidget)
    {
        m_timerWidget->SetHidden();
    }
}

//-----------------------------------------------------------------------------------
void GameMode::MarkTimerPaused()
{
    m_timerWidget->SetProperty<std::string>("Text", "Paused");
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
    if (!g_muteMusic)
    {
        AudioSystem::instance->PlayLoopingSound(m_backgroundMusic, 0.6f);
    }

    m_readyAnimFBOEffect = new Material(
        new ShaderProgram("Data\\Shaders\\fixedVertexFormat.vert", "Data\\Shaders\\Post\\readyAnimation.frag"),
        RenderState(RenderState::DepthTestingMode::OFF, RenderState::FaceCullingMode::RENDER_BACK_FACES, RenderState::BlendMode::ALPHA_BLEND)
        );

    m_readyAnimFBOEffect->SetFloatUniform("gEffectDurationSeconds", ANIMATION_LENGTH_SECONDS);
    m_readyAnimFBOEffect->SetVec4Uniform("gWipeColor", RGBA::BLACK.ToVec4());
    m_readyAnimFBOEffect->SetNormalTexture(ResourceDatabase::instance->GetSpriteResource("ReadyScreen")->m_texture);
    m_readyAnimFBOEffect->SetFloatUniform("gEffectTime", GetCurrentTimeSeconds());
    SpriteGameRenderer::instance->AddEffectToLayer(m_readyAnimFBOEffect, TheGame::UI_LAYER);

    m_modeTitleRenderable = new TextRenderable2D(m_modeTitleText, Transform2D(Vector2(0.0f, 0.9f)), TheGame::TEXT_LAYER, true);
    m_getReadyRenderable = new TextRenderable2D(m_modeDescriptionText, Transform2D(Vector2(0.0f, -3.3f)), TheGame::TEXT_LAYER, true);
    m_modeTitleRenderable->m_color = m_readyTextColor;
    m_getReadyRenderable->m_color = m_readyTextColor;
    m_modeTitleRenderable->m_fontSize = 1.2f;
    m_getReadyRenderable->m_fontSize = 0.5f;
    m_modeTitleRenderable->Disable();
    m_getReadyRenderable->Disable();
    HideBackground();
}

//-----------------------------------------------------------------------------------
void GameMode::UpdateReadyAnim(float deltaSeconds)
{
    if (g_secondsInState >= 0.5f)
    {
        m_modeTitleRenderable->Enable();
    }
    if (g_secondsInState >= 1.0f)
    {
        m_getReadyRenderable->Enable();
    }
}

//-----------------------------------------------------------------------------------
void GameMode::CleanupReadyAnim()
{
    SpriteGameRenderer::instance->RemoveEffectFromLayer(m_readyAnimFBOEffect, TheGame::UI_LAYER);
    delete m_readyAnimFBOEffect->m_shaderProgram;
    delete m_readyAnimFBOEffect;
    m_readyAnimFBOEffect = nullptr;
    delete m_modeTitleRenderable;
    m_modeTitleRenderable = nullptr;
    delete m_getReadyRenderable;
    m_getReadyRenderable = nullptr;
}

//-----------------------------------------------------------------------------------
void GameMode::ClearVortexPositions()
{
    for (Entity* entity : m_entities)
    {
        for (int i = 0; i < s_currentVortexId; ++i)
        {
            entity->SetVortexShaderPosition(Vector2(20000.0f, 20000.0f), i, 1.0f);
        }
    }

    s_currentVortexId = 0;
}

//-----------------------------------------------------------------------------------
void GameMode::InitializePlayerData()
{
    for (PlayerShip* player : m_players)
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
void GameMode::RankPlayers()
{
    int* scores = new int[TheGame::instance->m_numberOfPlayers];
    for (unsigned int i = 0; i < m_players.size(); ++i)
    {
        scores[i] = INT_MIN;
        PlayerShip* ship = m_players[i];
        DefaultPlayerStats* stats = m_playerStats[ship];
        scores[i] = stats->m_numKills - stats->m_numDeaths;
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