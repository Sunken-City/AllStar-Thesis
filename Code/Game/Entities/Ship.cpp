#include "Game/Entities/Ship.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Engine/Input/InputMap.hpp"
#include "Engine/Input/InputValues.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
Ship::Ship(Pilot* pilot)
    : Entity()
    , m_timeSinceLastShot(0.0f)
    , m_pilot(pilot)
{
    m_baseStats.braking = 0.9f;
}

//-----------------------------------------------------------------------------------
Ship::~Ship()
{
}

//-----------------------------------------------------------------------------------
void Ship::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    m_timeSinceLastShot += deltaSeconds;

    if (m_pilot)
    {
        UpdateMotion(deltaSeconds);
        UpdateShooting();

        if (m_pilot->m_inputMap.FindInputValue("Suicide")->WasJustPressed())
        {
            m_isDead = true;
            Die();
        }
    }
}

//-----------------------------------------------------------------------------------
void Ship::UpdateShooting()
{
    InputMap& input = m_pilot->m_inputMap;
    Vector2 shootDirection = input.GetVector2("ShootRight", "ShootUp");
    bool isShooting = input.FindInputValue("Shoot")->IsDown();
    if (shootDirection != Vector2::ZERO)
    {
        SetRotation(shootDirection.GetDirectionDegreesFromNormalizedVector());
    }

    if (isShooting && !m_lockMovement)
    {
        if (m_weapon)
        {
            m_weapon->AttemptFire();
        }
        else
        {
            float secondsPerShot = 1.0f / GetRateOfFireStat();
            if (m_timeSinceLastShot > secondsPerShot)
            {
                TheGame::instance->m_currentGameMode->SpawnBullet(this);
                m_timeSinceLastShot = 0.0f;
            }
        }
    }
}

//-----------------------------------------------------------------------------------
void Ship::ResolveCollision(Entity* otherEntity)
{
    Entity::ResolveCollision(otherEntity);
}

//-----------------------------------------------------------------------------------
void Ship::UpdateMotion(float deltaSeconds)
{
    const float speedSanityMultiplier = 3.0f / 1.0f;
    InputMap& input = m_pilot->m_inputMap;
    Vector2 inputDirection = input.GetVector2("Right", "Up");

    if (m_lockMovement)
    {
        return;
    }

    //Calculate Acceleration components
    Vector2 velocityDir = m_velocity.CalculateMagnitude() < 0.01f ? inputDirection.GetNorm() : m_velocity.GetNorm();
    Vector2 perpindicularVelocityDir(-velocityDir.y, velocityDir.x);
    float accelerationDot = Vector2::Dot(inputDirection, velocityDir);
    float accelerationMultiplier = (accelerationDot >= 0.0f) ? GetAccelerationStat() : GetHandlingStat();
    Vector2 accelerationComponent = accelerationComponent = velocityDir * accelerationDot * accelerationMultiplier;
    Vector2 agilityComponent = perpindicularVelocityDir * Vector2::Dot(inputDirection, perpindicularVelocityDir) * GetHandlingStat();

    //Calculate velocity
    Vector2 totalAcceleration = accelerationComponent + agilityComponent;
    m_velocity += totalAcceleration;
    m_velocity *= m_baseStats.braking; // +(0.1f * GetBrakingStat());
    m_velocity.ClampMagnitude(GetTopSpeedStat() * speedSanityMultiplier);

    Vector2 attemptedPosition = m_transform.position + (m_velocity * deltaSeconds);
    SetPosition(attemptedPosition);
}

//-----------------------------------------------------------------------------------
void Ship::TakeDamage(float damage)
{
    static SoundID hitSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Missle_02.wav");
    float currHp = m_currentHp;
    Entity::TakeDamage(damage);
    if (currHp != m_currentHp)
    {
        TheGame::instance->m_currentGameMode->PlaySoundAt(hitSound, GetPosition(), m_hitSoundMaxVolume);
    }
}

//-----------------------------------------------------------------------------------
void Ship::Die()
{
    static SoundID deathSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/trashExplosion.ogg");
    Entity::Die();
    TheGame::instance->m_currentGameMode->PlaySoundAt(deathSound, GetPosition(), m_hitSoundMaxVolume);
    ParticleSystem::PlayOneShotParticleEffect("Death", TheGame::PLAYER_LAYER, GetPosition(), 0.0f);

//     ResourceDatabase::instance->GetParticleSystemResource("DeadShip")->m_emitterDefinitions[0]->m_spriteResource = m_sprite->m_spriteResource;
//     ResourceDatabase::instance->GetParticleSystemResource("DeadShip")->m_emitterDefinitions[0]->m_initialScalePerParticle = m_sprite->m_scale;
//     ParticleSystem::PlayOneShotParticleEffect("DeadShip", TheGame::GRAVEYARD_LAYER, GetPosition(), m_transform.rotationDegrees);
}
