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
    , m_secondsSinceLastFiredWeapon(0.0f)
    , m_pilot(pilot)
{
    m_baseStats.braking = 0.9f;

    SetShieldHealth(CalculateShieldCapacityValue());
}

//-----------------------------------------------------------------------------------
Ship::~Ship()
{
}

//-----------------------------------------------------------------------------------
void Ship::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    m_secondsSinceLastFiredWeapon += deltaSeconds;
    RegenerateShield(deltaSeconds);

    if (m_pilot)
    {
        UpdateMotion(deltaSeconds);
        UpdateShooting();

        if (m_pilot->m_inputMap.FindInputValue("Suicide")->WasJustPressed())
        {
            TakeDamage(m_shieldHealth);
//             m_isDead = true;
//             Die();
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
            float secondsPerWeaponFire = 1.0f / CalculateRateOfFireValue();
            if (m_secondsSinceLastFiredWeapon > secondsPerWeaponFire)
            {
                TheGame::instance->m_currentGameMode->SpawnBullet(this);
                m_secondsSinceLastFiredWeapon = 0.0f;
            }
        }
    }
}

//-----------------------------------------------------------------------------------
void Ship::RegenerateShield(float deltaSeconds)
{
    const float SECONDS_BEFORE_SHIELD_REGEN_RESTARTS = 3.0f;
    if (m_timeSinceLastHit > SECONDS_BEFORE_SHIELD_REGEN_RESTARTS)
    {
        float regenPointsThisFrame = CalculateShieldRegenValue() * deltaSeconds;
        SetShieldHealth(m_shieldHealth + regenPointsThisFrame);
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
    float accelerationMultiplier = (accelerationDot >= 0.0f) ? CalculateAccelerationValue() : CalculateHandlingValue();
    Vector2 accelerationComponent = accelerationComponent = velocityDir * accelerationDot * accelerationMultiplier;
    Vector2 agilityComponent = perpindicularVelocityDir * Vector2::Dot(inputDirection, perpindicularVelocityDir) * CalculateHandlingValue();

    //Calculate velocity
    Vector2 totalAcceleration = accelerationComponent + agilityComponent;
    m_velocity += totalAcceleration;
    m_velocity *= m_baseStats.braking; // +(0.1f * CalculateBrakingValue());
    m_velocity.ClampMagnitude(CalculateTopSpeedValue());

    Vector2 attemptedPosition = m_transform.position + (m_velocity * deltaSeconds);
    SetPosition(attemptedPosition);
}

//-----------------------------------------------------------------------------------
void Ship::TakeDamage(float damage)
{
    static SoundID hitHullSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Missle_02.wav");
    static SoundID hitShieldSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Shield_07.wav");
    static SoundID brokeShieldSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Shield_08.wav");
    float currentHp = m_currentHp;
    float currentShieldCapacity = m_shieldHealth;

    Entity::TakeDamage(damage);
    if (currentShieldCapacity != m_shieldHealth)
    {
        if (m_shieldHealth != 0.0f)
        {
            TheGame::instance->m_currentGameMode->PlaySoundAt(hitShieldSound, GetPosition(), m_hitSoundMaxVolume);
        }
        else
        {
            TheGame::instance->m_currentGameMode->PlaySoundAt(brokeShieldSound, GetPosition(), m_hitSoundMaxVolume);
        }
    }
    else if (currentHp != m_currentHp)
    {
        TheGame::instance->m_currentGameMode->PlaySoundAt(hitHullSound, GetPosition(), m_hitSoundMaxVolume);
    }
}

//-----------------------------------------------------------------------------------
void Ship::Die()
{
    static SoundID deathSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/trashExplosion.ogg");
    Entity::Die();
    TheGame::instance->m_currentGameMode->PlaySoundAt(deathSound, GetPosition(), m_hitSoundMaxVolume);
    ParticleSystem::PlayOneShotParticleEffect("Death", TheGame::PLAYER_LAYER, GetPosition(), 0.0f);
}
