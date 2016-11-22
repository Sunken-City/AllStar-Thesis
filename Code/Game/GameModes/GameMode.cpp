#include "Game/GameModes/GameMode.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/StateMachine.hpp"
#include "Game/Entities/Projectile.hpp"
#include "Game/Entities/Pickup.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Audio/Audio.hpp"

//-----------------------------------------------------------------------------------
GameMode::GameMode(const std::string& arenaBackgroundImage)
    : m_arenaBackground(new Sprite(arenaBackgroundImage, TheGame::BACKGROUND_LAYER))
{
    m_arenaBackground->m_scale = Vector2(10.0f, 10.0f);
    SpriteGameRenderer::instance->SetWorldBounds(m_arenaBackground->GetBounds());

}

//-----------------------------------------------------------------------------------
GameMode::~GameMode()
{
    delete m_arenaBackground;
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
    return m_arenaBackground->GetBounds().GetRandomPointInside();
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
    return m_arenaBackground->GetBounds();
}

//-----------------------------------------------------------------------------------
void GameMode::SpawnBullet(Ship* creator)
{
    static SoundID bulletSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Bullets/SFX_Weapon_Fire_Single_02.wav");
    m_newEntities.push_back(new Projectile(creator));

    float distance = MathUtils::CalcDistSquaredBetweenPoints(SpriteGameRenderer::instance->GetCameraPositionInWorld(), creator->GetPosition());
    float attenuationVolume = 1.0f - (distance / 100.0f);
    AudioSystem::instance->PlaySound(bulletSound, attenuationVolume);
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
    SpriteGameRenderer::instance->SetWorldBounds(m_arenaBackground->GetBounds());
}
