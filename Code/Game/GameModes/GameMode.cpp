#include "Game/GameModes/GameMode.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/StateMachine.hpp"
#include "Game/Entities/Projectile.hpp"
#include "Game/Entities/Pickup.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "../Entities/PlayerShip.hpp"

//-----------------------------------------------------------------------------------
GameMode::GameMode(const std::string& arenaBackgroundImage)
    : m_arenaBackground(new Sprite(arenaBackgroundImage, TheGame::BACKGROUND_LAYER))
{
    m_backgroundMusic = AudioSystem::instance->CreateOrGetSound("Data/SFX/Music/PlaceholderMusic1.m4a");
}

//-----------------------------------------------------------------------------------
GameMode::~GameMode()
{
    delete m_arenaBackground;
}

//-----------------------------------------------------------------------------------
void GameMode::Initialize()
{
    if (!m_muteMusic)
    {
        AudioSystem::instance->PlayLoopingSound(m_backgroundMusic, 0.6f);
    }
}

//-----------------------------------------------------------------------------------
void GameMode::CleanUp()
{
    AudioSystem::instance->StopSound(m_backgroundMusic);
}

//-----------------------------------------------------------------------------------
void GameMode::Update(float deltaSeconds)
{
    if (m_isPlaying)
    {
        m_timerSecondsElapsed += deltaSeconds;
    }
    if (m_timerSecondsElapsed >= m_gameLengthSeconds)
    {
        m_isPlaying = false; 
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
void GameMode::SpawnBullet(Ship* creator)
{
    static SoundID bulletSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Bullets/SFX_Weapon_Fire_Single_02.wav");
    m_newEntities.push_back(new Projectile(creator, creator->CalculateDamageValue(), creator->CalculateShieldDisruptionValue(), creator->CalculateShieldPenetrationValue()));

    Vector2 shotPosition = creator->GetMuzzlePosition();
    PlaySoundAt(bulletSound, shotPosition, 0.5f);
    ParticleSystem::PlayOneShotParticleEffect("MuzzleFlash", TheGame::PLAYER_BULLET_LAYER, shotPosition, 0.0f);
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
    m_arenaBackground->m_scale = scale;
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
