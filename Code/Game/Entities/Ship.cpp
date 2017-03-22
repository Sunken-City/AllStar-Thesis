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
#include "../GameCommon.hpp"
#include "Engine/Math/Noise.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Props/ShipDebris.hpp"

//-----------------------------------------------------------------------------------
Ship::Ship(Pilot* pilot)
    : Entity()
    , m_secondsSinceLastFiredWeapon(0.0f)
    , m_pilot(pilot)
    , m_shipTrail(new RibbonParticleSystem("ShipTrail", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(), &m_transform))
    , m_smokeDamage(new ParticleSystem("SmokeTrail", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(), &m_transform))
{
    SetShieldHealth(CalculateShieldCapacityValue());
    m_collisionSpriteResource = ResourceDatabase::instance->GetSpriteResource("Explosion");
    m_shieldCollisionSpriteResource = ResourceDatabase::instance->GetSpriteResource("ParticleGreen");
    m_smokeDamage->Disable();
    m_shieldSprite->m_transform.IgnoreParentScale();
}

//-----------------------------------------------------------------------------------
Ship::~Ship()
{
    ParticleSystem::DestroyImmediately(m_shipTrail);
    ParticleSystem::DestroyImmediately(m_smokeDamage);
}

//-----------------------------------------------------------------------------------
void Ship::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    m_secondsSinceLastFiredWeapon += deltaSeconds;
    RegenerateShield(deltaSeconds);
    FlickerShield(deltaSeconds);
    ApplyShotDeflection();

    if (m_pilot)
    {
        UpdateMotion(deltaSeconds);
        UpdateShooting();

        m_velocity.CalculateMagnitudeSquared() < 0.1f ? m_shipTrail->Pause() : m_shipTrail->Unpause();

        InputValue* suicideInput = m_pilot->m_inputMap.FindInputValue("Suicide");
        if (suicideInput && suicideInput->WasJustPressed())
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

    if (shootDirection.CalculateMagnitudeSquared() > DEADZONE_BEFORE_ROTATION_SQUARED)
    {
        SetRotation(shootDirection.GetDirectionDegrees());
    }

    if (isShooting && !m_lockMovement)
    {
        if (m_weapon)
        {
            if (m_weapon->AttemptFire(this))
            {
                ApplyImpulse(-shootDirection * m_weapon->GetKnockbackMagnitude());
            }
        }
        else
        {
            if (m_defaultWeapon.AttemptFire(this))
            {
                ApplyImpulse(-shootDirection * m_defaultWeapon.GetKnockbackMagnitude());
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
void Ship::FlickerShield(float deltaSeconds)
{
    static int index = 0;
    UNUSED(deltaSeconds);
    float ratio = m_currentShieldHealth / CalculateShieldCapacityValue();
    float noiseValue = Get1dNoiseNegOneToOne(++index);
    float mappedNoiseValue = MathUtils::RangeMap(noiseValue, -1.0f, 1.0f, 0.0f, 1.0f);
    float alphaValue = Max<float>(ratio, noiseValue);
    m_shieldSprite->m_tintColor.SetAlphaFloat(alphaValue);

    float rotationThisFrame = (ANGULAR_VELOCITY * deltaSeconds) * ratio;
    m_shieldSprite->m_transform.SetRotationDegrees(m_shieldSprite->m_transform.GetLocalRotationDegrees() + rotationThisFrame);
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

    //Rotate the ship towards it's direction of motion if it's not being rotated.
    Vector2 shootDirection = input.GetVector2("ShootRight", "ShootUp");
    bool isShooting = input.FindInputValue("Shoot")->IsDown();

    if (shootDirection.CalculateMagnitudeSquared() < DEADZONE_BEFORE_ROTATION_SQUARED && !isShooting)
    {
        float currRotation = GetRotation();
        float angleDifference = MathUtils::CalcShortestAngularDisplacement(currRotation, inputDirection.GetDirectionDegrees()/*m_velocity.GetDirectionDegreesFromNormalizedVector()*/);
        float desiredRotation = currRotation + angleDifference;
        SetRotation(MathUtils::Lerp(0.20f, currRotation, desiredRotation));
    }
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

        float halfHealth = CalculateHpValue() * 0.5f;
        if (m_currentHp < halfHealth && !m_smokeDamage->m_isEnabled)
        {
            m_smokeDamage->Enable();
        }
    }

    return damageTaken;
}

//-----------------------------------------------------------------------------------
void Ship::Heal(float healValue /*= 99999999.0f*/)
{
    Entity::Heal(healValue);
    
    float halfHealth = CalculateHpValue() * 0.5f;
    if (m_currentHp > halfHealth && m_smokeDamage->m_isEnabled)
    {
        m_smokeDamage->Disable();
    }
}

//-----------------------------------------------------------------------------------
void Ship::Die()
{
    static SoundID deathSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/Hit/trashExplosion.ogg");
    Entity::Die();
    TheGame::instance->m_currentGameMode->PlaySoundAt(deathSound, GetPosition(), m_hitSoundMaxVolume);
    m_smokeDamage->Disable();
    ShipDebris* debris = new ShipDebris(m_transform, m_sprite->m_spriteResource, m_velocity);
    ParticleSystem::PlayOneShotParticleEffect("Death", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &debris->m_transform);
    GameMode::GetCurrent()->SpawnEntityInGameWorld(debris);
}

//-----------------------------------------------------------------------------------
bool Ship::FlushParticleTrailIfExists()
{
    m_shipTrail->Flush();
    return true;
}

//-----------------------------------------------------------------------------------
Vector2 Ship::GetMuzzlePosition()
{
    return GetPosition() + Vector2::CreateFromPolar(m_muzzleOffsetMagnitude, -m_transform.GetWorldRotationDegrees() + 90.0f);
}

//-----------------------------------------------------------------------------------
const SpriteResource* Ship::GetCollisionSpriteResource()
{
    return HasShield() ? m_shieldCollisionSpriteResource : m_collisionSpriteResource;
}

//-----------------------------------------------------------------------------------
void Ship::SetVortexShaderPosition(const Vector2& vortexPosition, int vortexID, float vortexRadii)
{
    Entity::SetVortexShaderPosition(vortexPosition, vortexID, vortexRadii);
    m_shieldSprite->m_material->SetVec3Uniform(Stringf("gWarpPositions[%i]", vortexID).c_str(), Vector3(vortexPosition, 0.0f));
    m_shieldSprite->m_material->SetFloatUniform(Stringf("gVortexRadii[%i]", vortexID).c_str(), vortexRadii);
    m_shipTrail->m_emitters[0]->m_materialOverride->SetVec3Uniform(Stringf("gWarpPositions[%i]", vortexID).c_str(), Vector3(vortexPosition, 0.0f));
    m_shipTrail->m_emitters[0]->m_materialOverride->SetFloatUniform(Stringf("gVortexRadii[%i]", vortexID).c_str(), vortexRadii);
}
