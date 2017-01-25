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

//-----------------------------------------------------------------------------------
GameMode::GameMode(const std::string& arenaBackgroundImage)
    : m_arenaBackground(new Sprite(arenaBackgroundImage, TheGame::BACKGROUND_LAYER))
{
    m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/SFX/Music/PlaceholderMusic1.m4a");
}

//-----------------------------------------------------------------------------------
GameMode::~GameMode()
{
    StopPlaying();
    delete m_arenaBackground;
    m_arenaBackground = nullptr;
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
Vector2 GameMode::GetRandomLocationInArena()
{
    return GetArenaBounds().GetRandomPointInside();
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
    float clampedVolume = std::min(attenuationVolume, maxVolume);
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
