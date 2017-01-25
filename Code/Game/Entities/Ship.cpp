#include "Game/Entities/Ship.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Engine/Input/InputMap.hpp"
#include "Engine/Input/InputValues.hpp"
#include "Game/Items/Weapons/LaserGun.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"

//-----------------------------------------------------------------------------------
Ship::Ship(Pilot* pilot)
    : Entity()
    , m_secondsSinceLastFiredWeapon(0.0f)
    , m_pilot(pilot)
    , m_shipTrail(new RibbonParticleSystem("ShipTrail", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(), &m_transform))
{
    SetShieldHealth(CalculateShieldCapacityValue());
    m_collisionSpriteResource = ResourceDatabase::instance->GetSpriteResource("ParticleGrey");
    m_shieldCollisionSpriteResource = ResourceDatabase::instance->GetSpriteResource("ParticleGreen");
    m_weapon = new LaserGun();
}

//-----------------------------------------------------------------------------------
Ship::~Ship()
{
    ParticleSystem::DestroyImmediately(m_shipTrail);
}

//-----------------------------------------------------------------------------------
void Ship::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    m_secondsSinceLastFiredWeapon += deltaSeconds;
    RegenerateShield(deltaSeconds);
    ApplyShotDeflection();

    if (m_pilot)
    {
        UpdateMotion(deltaSeconds);
        UpdateShooting();

        m_velocity.CalculateMagnitudeSquared() < 0.1f ? m_shipTrail->Pause() : m_shipTrail->Unpause();

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
    static const float deadzoneBeforeRotation = 0.3f;
    static const float deadzoneBeforeRotationSquared = deadzoneBeforeRotation * deadzoneBeforeRotation;
    InputMap& input = m_pilot->m_inputMap;
    Vector2 shootDirection = input.GetVector2("ShootRight", "ShootUp");
    bool isShooting = input.FindInputValue("Shoot")->IsDown();

    if (shootDirection.CalculateMagnitudeSquared() > deadzoneBeforeRotationSquared)
    {
        SetRotation(shootDirection.GetDirectionDegreesFromNormalizedVector());
    }

    if (isShooting && !m_lockMovement)
    {
        if (m_weapon)
        {
            m_weapon->AttemptFire(this);
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
        SetShieldHealth(m_currentShieldHealth + regenPointsThisFrame);
    }
}

//-----------------------------------------------------------------------------------
void Ship::ApplyShotDeflection()
{
    static const float DEADSHOT_DOT_TOLERANCE = fabs(MathUtils::SinDegrees(5.0f));
    const float DEFLECTION_RADIUS = 4.0f;
    const float DEFLECTION_RADIUS_SQUARED = DEFLECTION_RADIUS * DEFLECTION_RADIUS;
    GameMode* current = GameMode::GetCurrent();

    for (Entity* entity : current->m_entities)
    {
        if (entity->IsProjectile() && entity->m_owner != this)
        {
            Projectile* projectile = (Projectile*)entity;
            Vector2 bulletPos = projectile->GetPosition();
            float distBetweenShipAndProjectileSquared = MathUtils::CalcDistSquaredBetweenPoints(GetPosition(), bulletPos);

            if (distBetweenShipAndProjectileSquared < DEFLECTION_RADIUS_SQUARED)
            {
                Vector2 displacementFromBulletToShip = GetPosition() - bulletPos;
                Vector2 velocityPerpendicular = Vector2(-projectile->m_velocity.y, projectile->m_velocity.x);
                Vector2 displacementNormalized = displacementFromBulletToShip.GetNorm();
                Vector2 normalizedVelocity = velocityPerpendicular.GetNorm();

                float dotProduct = Vector2::Dot(displacementNormalized, normalizedVelocity);
                if (fabs(dotProduct) > DEADSHOT_DOT_TOLERANCE)
                {
                    Vector2 resolutionDirection = dotProduct > 0 ? -normalizedVelocity : normalizedVelocity;
                    float totalShotModificationConstant = CalculateShotDeflectionValue() - projectile->m_shotHoming;
                    projectile->ApplyImpulse(resolutionDirection * totalShotModificationConstant);
                }                
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
    Vector2 accelerationComponent = velocityDir * accelerationDot * accelerationMultiplier;
    Vector2 agilityComponent = perpindicularVelocityDir * Vector2::Dot(inputDirection, perpindicularVelocityDir) * CalculateHandlingValue();

    //Calculate velocity
    Vector2 totalAcceleration = accelerationComponent + agilityComponent;
    m_velocity += totalAcceleration;
    m_velocity *= CalculateBrakingValue();
    m_velocity.ClampMagnitude(CalculateTopSpeedValue());

    Vector2 attemptedPosition = m_transform.GetWorldPosition() + (m_velocity * deltaSeconds);
    SetPosition(attemptedPosition);
}

//-----------------------------------------------------------------------------------
float Ship::TakeDamage(float damage, float disruption /*= 1.0f*/)
{
    static SoundID hitHullSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Missle_02.wav");
    static SoundID hitShieldSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Shield_07.wav");
    static SoundID brokeShieldSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/SFX_Impact_Shield_08.wav");
    float currentHp = m_currentHp;
    float currentShieldCapacity = m_currentShieldHealth;
    
    float damageTaken = Entity::TakeDamage(damage, disruption);
    if (currentShieldCapacity != m_currentShieldHealth)
    {
        if (m_currentShieldHealth != 0.0f)
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

    return damageTaken;
}

//-----------------------------------------------------------------------------------
void Ship::Die()
{
    static SoundID deathSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/trashExplosion.ogg");
    Entity::Die();
    TheGame::instance->m_currentGameMode->PlaySoundAt(deathSound, GetPosition(), m_hitSoundMaxVolume);
    ParticleSystem::PlayOneShotParticleEffect("Death", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(GetPosition()));
}

//-----------------------------------------------------------------------------------
Vector2 Ship::GetMuzzlePosition()
{
    return GetPosition() + Vector2::CreateFromPolar(m_muzzleOffsetMagnitude, -m_sprite->m_transform.GetWorldRotationDegrees() + 90.0f);
}

//-----------------------------------------------------------------------------------
const SpriteResource* Ship::GetCollisionSpriteResource()
{
    return HasShield() ? m_shieldCollisionSpriteResource : m_collisionSpriteResource;
}
