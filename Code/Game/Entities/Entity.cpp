#include "Game/Entities/Entity.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Items/Chassis/Chassis.hpp"
#include "Game/Items/Actives/ActiveEffect.hpp"
#include "Game/Items/Passives/PassiveEffect.hpp"
#include "Game/StateMachine.hpp"
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"
#include <algorithm>
#include "Engine/Renderer/Material.hpp"
#include "TextSplash.hpp"
#include "PlayerShip.hpp"

Vector2 Entity::SHIELD_SCALE_FUDGE_VALUE = Vector2(0.1f);

//-----------------------------------------------------------------------------------
Entity::Entity()
    : m_sprite(nullptr)
    , m_shieldSprite(new Sprite("Shield", TheGame::SHIELD_LAYER, false))
    , m_currentHp(1.0f)
    , m_collisionRadius(1.0f)
    , m_age(0.0f)
    , m_isDead(false)
    , m_weapon(nullptr)
    , m_chassis(nullptr)
    , m_activeEffect(nullptr)
    , m_passiveEffect(nullptr)
    , m_baseStats(6.0f)
    , m_velocity(0.0f)
    , m_frictionValue(0.9f)
    , m_collidesWithBullets(true)
    , m_isInvincible(false)
    , m_owner(nullptr)
    , m_noCollide(false)
    , m_currentShieldHealth(0.0f)
{
    m_shieldSprite->m_transform.SetParent(&m_transform);
    m_shieldSprite->m_transform.IgnoreParentRotation();
}

//-----------------------------------------------------------------------------------
Entity::~Entity()
{
    if (m_sprite)
    {
        delete m_sprite;
    }
    delete m_shieldSprite;
    if (GetGameState() == GameState::ASSEMBLY_PLAYING)
    {
        DropInventory();
    }
    else
    {
        DeleteInventory();
    }
    if (m_weapon)
    {
        delete m_weapon;
    }
    if (m_chassis)
    {
        delete m_chassis;
    }
    if (m_passiveEffect)
    {
        delete m_passiveEffect;
    }
    if (m_activeEffect)
    {
        delete m_activeEffect;
    }
}

//-----------------------------------------------------------------------------------
void Entity::Update(float deltaSeconds)
{
    m_age += deltaSeconds;
    m_timeSinceLastHit += deltaSeconds;
    Vector2 accelerationDueToImpulses = m_sumOfImpulses / m_mass;
    m_velocity += (accelerationDueToImpulses * deltaSeconds);
    m_sumOfImpulses = Vector2::ZERO; //Only applied for a frame.
}

//-----------------------------------------------------------------------------------
bool Entity::IsCollidingWith(Entity* otherEntity)
{
    float distSquared = MathUtils::CalcDistSquaredBetweenPoints(GetPosition(), otherEntity->GetPosition());
    float sumOfRadii = m_collisionRadius + otherEntity->m_collisionRadius;
    return (distSquared < sumOfRadii * sumOfRadii);
}

//-----------------------------------------------------------------------------------
void Entity::ResolveCollision(Entity* otherEntity)
{
    if (m_isDead || otherEntity->m_isDead || m_noCollide || otherEntity->m_noCollide || otherEntity == m_owner || otherEntity->m_owner == this || (!m_collidesWithBullets && (otherEntity->IsProjectile() || IsProjectile())))
    {
        return;
    }

    Vector2 myPosition = GetPosition();
    Vector2 otherPosition = otherEntity->GetPosition();
    Vector2 displacementFromOtherToMe = myPosition - otherPosition;
    Vector2 directionFromOtherToMe = displacementFromOtherToMe.GetNorm();

    float distanceBetweenEntities = displacementFromOtherToMe.CalculateMagnitude();
    float sumOfRadii = m_collisionRadius + otherEntity->m_collisionRadius;
    float overlapDistance = sumOfRadii - distanceBetweenEntities;
    float pushDistance = overlapDistance * 0.5f;
    Vector2 myPositionCorrection = directionFromOtherToMe * pushDistance;
    if (!m_isImmobile)
    {
        SetPosition(myPosition + myPositionCorrection);
    }
    if (!otherEntity->m_isImmobile)
    {
        otherEntity->SetPosition(otherPosition - myPositionCorrection); 
    }

    //Take damage from the collision if anything does damage on contact
    if (m_collisionDamageAmount > 0.0f && otherEntity->CanTakeContactDamage())
    {
        otherEntity->TakeDamage(m_collisionDamageAmount);
    }
    if (otherEntity->m_collisionDamageAmount > 0.0f && CanTakeContactDamage())
    {
        TakeDamage(otherEntity->m_collisionDamageAmount);
    }

    if (otherEntity->IsDead() && otherEntity->IsPlayer() && IsPlayer())
    {
        PlayerShip* player = dynamic_cast<PlayerShip*>(this);
        PlayerShip* victim = dynamic_cast<PlayerShip*>(otherEntity);
        ASSERT_OR_DIE(player && victim, "Somehow got a player and victim to not be players.");
        GameMode::GetCurrent()->RecordPlayerKill(player, victim);
    }
    else if (IsDead() && IsPlayer() && otherEntity->IsPlayer())
    {
        PlayerShip* player = dynamic_cast<PlayerShip*>(otherEntity);
        PlayerShip* victim = dynamic_cast<PlayerShip*>(this);
        ASSERT_OR_DIE(player && victim, "Somehow got a player and victim to not be players.");
        GameMode::GetCurrent()->RecordPlayerKill(player, victim);
    }
}

//-----------------------------------------------------------------------------------
void Entity::ApplyImpulse(const Vector2& appliedAcceleration)
{
    m_sumOfImpulses += m_mass * appliedAcceleration;
}

//-----------------------------------------------------------------------------------
float Entity::TakeDamage(float damage, float disruption /*= 1.0f*/)
{
    if (m_isDead || m_isInvincible)
    {
        return 0.0f;
    }

    float damageDealt = 0.0f;
    float randomPercentage = MathUtils::GetRandomFloatFromZeroTo(1.0f) - 0.5f;
    damage += damage * randomPercentage;
    damage = MathUtils::Clamp(damage, 1.0f, FLT_MAX);
    randomPercentage *= 0.2f;
    float randomDegrees = MathUtils::GetRandomFloat(-70.0f, 70.0f);
    Vector2 velocity = Vector2::DegreesToDirection(randomDegrees, Vector2::ZERO_DEGREES_UP) * 2.0f;

    if (HasShield())
    {
        float adjustedDamage = damage + (damage * disruption);
        SetShieldHealth(m_currentShieldHealth - adjustedDamage);
        damageDealt = adjustedDamage;
        TextSplash::CreateTextSplash(Stringf("%i", static_cast<int>(damageDealt)), m_transform, velocity, RGBA(0.0f, 0.8f + randomPercentage, 1.0f, 1.0f));
    }
    else
    {
        m_currentHp -= damage;
        damageDealt = damage;
        if (ShowsDamageNumbers())
        {
            TextSplash::CreateTextSplash(Stringf("%i", static_cast<int>(damageDealt)), m_transform, velocity, RGBA(1.0f, 1.0f - (0.8f + randomPercentage), 0.0f, 1.0f));
        }
        if (m_currentHp <= 0.0f)
        {
            m_currentHp = 0.0f;
            Die();
        }
    }
    m_timeSinceLastHit = 0.0f;
    return damageDealt;
}

//-----------------------------------------------------------------------------------
void Entity::CalculateCollisionRadius()
{
    Vector2 virtualSize = m_sprite->m_spriteResource->m_virtualSize;
    Vector2 spriteScale = m_sprite->m_transform.GetWorldScale();

    float maxVirtualSize = Max(virtualSize.x, virtualSize.y);
    float maxSpriteScale = Max(spriteScale.x, spriteScale.y);
    maxVirtualSize *= 0.5f;
    m_collisionRadius = maxVirtualSize * maxSpriteScale;

    //Get the shield to match the scale of the entity, then add more to encompass it entirely.
    Vector2 scale = Vector2(m_collisionRadius * 3.0f) / (m_shieldSprite->m_spriteResource->m_virtualSize);
    m_shieldSprite->m_transform.SetScale(scale + SHIELD_SCALE_FUDGE_VALUE);
}

//-----------------------------------------------------------------------------------
void Entity::SetPosition(const Vector2& newPosition)
{
    Vector2 adjustedPosition = newPosition;

    if (m_staysWithinBounds)
    {
        AABB2 bounds = TheGame::instance->m_currentGameMode->GetArenaBounds();
        //We make the inverse minkowski box because we'd like to stay INSIDE the box
        AABB2 minkowskiBounds = AABB2(Vector2(bounds.mins.x + m_collisionRadius, bounds.mins.y + m_collisionRadius), Vector2(bounds.maxs.x - m_collisionRadius, bounds.maxs.y - m_collisionRadius));
        while (!minkowskiBounds.IsPointOnOrInside(adjustedPosition))
        {
            Vector2 distanceOutsideBounds = minkowskiBounds.GetSmallestOutToInResolutionVector(newPosition);
            adjustedPosition += distanceOutsideBounds;
        }
    }

    m_transform.SetPosition(adjustedPosition);
}

//-----------------------------------------------------------------------------------
void Entity::SetRotation(const float newDegreesRotation)
{
    m_transform.SetRotationDegrees(newDegreesRotation);
}

//-----------------------------------------------------------------------------------
void Entity::Heal(float healValue)
{
    m_currentHp += healValue;
    m_currentHp = MathUtils::Clamp(m_currentHp, 0.0f, CalculateHpValue());
}

//-----------------------------------------------------------------------------------
float Entity::GetTopSpeedStat()
{
    float topSpeed = m_baseStats.topSpeed;
    topSpeed += m_weapon ? m_weapon->m_statBonuses.topSpeed : 0.0f;
    topSpeed += m_chassis ? m_chassis->m_statBonuses.topSpeed : 0.0f;
    topSpeed += m_activeEffect ? m_activeEffect->m_statBonuses.topSpeed : 0.0f;
    topSpeed += m_passiveEffect ? m_passiveEffect->m_statBonuses.topSpeed : 0.0f;
    return Clamp<float>(topSpeed, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetAccelerationStat()
{
    float totalAcceleration = m_baseStats.acceleration;
    totalAcceleration += m_weapon ? m_weapon->m_statBonuses.acceleration : 0.0f;
    totalAcceleration += m_chassis ? m_chassis->m_statBonuses.acceleration : 0.0f;
    totalAcceleration += m_activeEffect ? m_activeEffect->m_statBonuses.acceleration : 0.0f;
    totalAcceleration += m_passiveEffect ? m_passiveEffect->m_statBonuses.acceleration : 0.0f;
    return Clamp<float>(totalAcceleration, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetHandlingStat()
{
    float handling = m_baseStats.handling;
    handling += m_weapon ? m_weapon->m_statBonuses.handling : 0.0f;
    handling += m_chassis ? m_chassis->m_statBonuses.handling : 0.0f;
    handling += m_activeEffect ? m_activeEffect->m_statBonuses.handling : 0.0f;
    handling += m_passiveEffect ? m_passiveEffect->m_statBonuses.handling : 0.0f;
    return Clamp<float>(handling, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetBrakingStat()
{
    float braking = m_baseStats.braking;
    braking += m_weapon ? m_weapon->m_statBonuses.braking : 0.0f;
    braking += m_chassis ? m_chassis->m_statBonuses.braking : 0.0f;
    braking += m_activeEffect ? m_activeEffect->m_statBonuses.braking : 0.0f;
    braking += m_passiveEffect ? m_passiveEffect->m_statBonuses.braking : 0.0f;
    return Clamp<float>(braking, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetDamageStat()
{
    float damage = m_baseStats.damage;
    damage += m_weapon ? m_weapon->m_statBonuses.damage : 0.0f;
    damage += m_chassis ? m_chassis->m_statBonuses.damage : 0.0f;
    damage += m_activeEffect ? m_activeEffect->m_statBonuses.damage : 0.0f;
    damage += m_passiveEffect ? m_passiveEffect->m_statBonuses.damage : 0.0f;
    return Clamp<float>(damage, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetShieldDisruptionStat()
{
    float shieldDisruption = m_baseStats.shieldDisruption;
    shieldDisruption += m_weapon ? m_weapon->m_statBonuses.shieldDisruption : 0.0f;
    shieldDisruption += m_chassis ? m_chassis->m_statBonuses.shieldDisruption : 0.0f;
    shieldDisruption += m_activeEffect ? m_activeEffect->m_statBonuses.shieldDisruption : 0.0f;
    shieldDisruption += m_passiveEffect ? m_passiveEffect->m_statBonuses.shieldDisruption : 0.0f;
    return Clamp<float>(shieldDisruption, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetShotHomingStat()
{
    float shotHoming = m_baseStats.shotHoming;
    shotHoming += m_weapon ? m_weapon->m_statBonuses.shotHoming : 0.0f;
    shotHoming += m_chassis ? m_chassis->m_statBonuses.shotHoming : 0.0f;
    shotHoming += m_activeEffect ? m_activeEffect->m_statBonuses.shotHoming : 0.0f;
    shotHoming += m_passiveEffect ? m_passiveEffect->m_statBonuses.shotHoming : 0.0f;
    return Clamp<float>(shotHoming, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetRateOfFireStat()
{
    float rateOfFire = m_baseStats.rateOfFire;
    rateOfFire += m_weapon ? m_weapon->m_statBonuses.rateOfFire : 0.0f;
    rateOfFire += m_chassis ? m_chassis->m_statBonuses.rateOfFire : 0.0f;
    rateOfFire += m_activeEffect ? m_activeEffect->m_statBonuses.rateOfFire : 0.0f;
    rateOfFire += m_passiveEffect ? m_passiveEffect->m_statBonuses.rateOfFire : 0.0f;
    return Clamp<float>(rateOfFire, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetHpStat()
{
    float hp = m_baseStats.hp;
    hp += m_weapon ? m_weapon->m_statBonuses.hp : 0.0f;
    hp += m_chassis ? m_chassis->m_statBonuses.hp : 0.0f;
    hp += m_activeEffect ? m_activeEffect->m_statBonuses.hp : 0.0f;
    hp += m_passiveEffect ? m_passiveEffect->m_statBonuses.hp : 0.0f;
    return Clamp<float>(hp, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetShieldCapacityStat()
{
    float shieldCapacity = m_baseStats.shieldCapacity;
    shieldCapacity += m_weapon ? m_weapon->m_statBonuses.shieldCapacity : 0.0f;
    shieldCapacity += m_chassis ? m_chassis->m_statBonuses.shieldCapacity : 0.0f;
    shieldCapacity += m_activeEffect ? m_activeEffect->m_statBonuses.shieldCapacity : 0.0f;
    shieldCapacity += m_passiveEffect ? m_passiveEffect->m_statBonuses.shieldCapacity : 0.0f;
    return Clamp<float>(shieldCapacity, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetShieldRegenStat()
{
    float shieldRegen = m_baseStats.shieldRegen;
    shieldRegen += m_weapon ? m_weapon->m_statBonuses.shieldRegen : 0.0f;
    shieldRegen += m_chassis ? m_chassis->m_statBonuses.shieldRegen : 0.0f;
    shieldRegen += m_activeEffect ? m_activeEffect->m_statBonuses.shieldRegen : 0.0f;
    shieldRegen += m_passiveEffect ? m_passiveEffect->m_statBonuses.shieldRegen : 0.0f;
    return Clamp<float>(shieldRegen, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::GetShotDeflectionStat()
{
    float shotDeflection = m_baseStats.shotDeflection;
    shotDeflection += m_weapon ? m_weapon->m_statBonuses.shotDeflection : 0.0f;
    shotDeflection += m_chassis ? m_chassis->m_statBonuses.shotDeflection : 0.0f;
    shotDeflection += m_activeEffect ? m_activeEffect->m_statBonuses.shotDeflection : 0.0f;
    shotDeflection += m_passiveEffect ? m_passiveEffect->m_statBonuses.shotDeflection : 0.0f;
    return Clamp<float>(shotDeflection, Stats::MIN_LEVEL, Stats::MAX_LEVEL);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateTopSpeedValue()
{
    float topSpeedStat = GetTopSpeedStat();
    float topSpeedStatZeroOne = MathUtils::RangeMap(topSpeedStat, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    topSpeedStatZeroOne = MathUtils::SmoothStep(topSpeedStatZeroOne);
    float topSpeedStatAdjusted = MathUtils::RangeMap(topSpeedStatZeroOne, 0.0f, 1.0f, Stats::MIN_SPEED_VALUE, Stats::MAX_SPEED_VALUE);
    return topSpeedStatAdjusted;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateAccelerationValue()
{
    float accelerationStat = GetAccelerationStat();
    float accelerationStatZeroOne = MathUtils::RangeMap(accelerationStat, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    accelerationStatZeroOne = MathUtils::SmoothStop2(accelerationStatZeroOne);
    float accelerationStatAdjusted = MathUtils::RangeMap(accelerationStatZeroOne, 0.0f, 1.0f, Stats::MIN_ACCELERATION_VALUE, Stats::MAX_ACCELERATION_VALUE);
    return accelerationStatAdjusted;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateHandlingValue()
{
    float handlingStat = GetHandlingStat();
    float handlingStatZeroOne = MathUtils::RangeMap(handlingStat, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    handlingStatZeroOne = MathUtils::SmoothStop2(handlingStatZeroOne);
    float handlingStatAdjusted = MathUtils::RangeMap(handlingStatZeroOne, 0.0f, 1.0f, Stats::MIN_HANDLING_VALUE, Stats::MAX_HANDLING_VALUE);
    return handlingStatAdjusted;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateBrakingValue()
{
    float brakingStat = GetBrakingStat();
    float brakingStatZeroOne = MathUtils::RangeMap(brakingStat, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    brakingStatZeroOne = MathUtils::SmoothStop2(brakingStatZeroOne);
    float brakingStatAdjusted = MathUtils::RangeMap(brakingStatZeroOne, 0.0f, 1.0f, Stats::MIN_BRAKING_VALUE, Stats::MAX_BRAKING_VALUE);
    return brakingStatAdjusted;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateDamageValue()
{
    float statValue = GetDamageStat();
    float statZeroToOne = MathUtils::RangeMap(statValue, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    statZeroToOne = MathUtils::SmoothStop2(statZeroToOne);
    float adjustedStat = MathUtils::RangeMap(statZeroToOne, 0.0f, 1.0f, Stats::MIN_DAMAGE_VALUE, Stats::MAX_DAMAGE_VALUE);
    return adjustedStat;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateShieldDisruptionValue()
{
    float statValue = GetShieldDisruptionStat();
    float statZeroToOne = MathUtils::RangeMap(statValue, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    statZeroToOne = MathUtils::SmoothStop2(statZeroToOne);
    float adjustedStat = MathUtils::RangeMap(statZeroToOne, 0.0f, 1.0f, Stats::MIN_DISRUPTION_PERCENTAGE, Stats::MAX_DISRUPTION_PERCENTAGE);
    return adjustedStat;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateShotHomingValue()
{
    float statValue = GetShotHomingStat();
    float statZeroToOne = MathUtils::RangeMap(statValue, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    statZeroToOne = MathUtils::SmoothStop2(statZeroToOne);
    float adjustedStat = MathUtils::RangeMap(statZeroToOne, 0.0f, 1.0f, Stats::MIN_SHOT_HOMING_VALUE, Stats::MAX_SHOT_HOMING_VALUE);
    return adjustedStat;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateRateOfFireValue()
{
    float statValue = GetRateOfFireStat();
    float statZeroToOne = MathUtils::RangeMap(statValue, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    statZeroToOne = MathUtils::SmoothStop2(statZeroToOne);
    float adjustedStat = MathUtils::RangeMap(statZeroToOne, 0.0f, 1.0f, Stats::MIN_RATE_OF_FIRE, Stats::MAX_RATE_OF_FIRE);
    return adjustedStat;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateHpValue()
{
    float statValue = GetHpStat();
    float statZeroToOne = MathUtils::RangeMap(statValue, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    statZeroToOne = MathUtils::SmoothStep(statZeroToOne);
    float adjustedStat = MathUtils::RangeMap(statZeroToOne, 0.0f, 1.0f, Stats::MIN_HP_VALUE, Stats::MAX_HP_VALUE);
    return adjustedStat;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateShieldCapacityValue()
{
    float statValue = GetShieldCapacityStat();
    float statZeroToOne = MathUtils::RangeMap(statValue, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    statZeroToOne = MathUtils::SmoothStep(statZeroToOne);
    float adjustedStat = MathUtils::RangeMap(statZeroToOne, 0.0f, 1.0f, Stats::MIN_CAPACITY_VALUE, Stats::MAX_CAPACITY_VALUE);
    return adjustedStat;
}

//-----------------------------------------------------------------------------------
//This metric is in shield hitpoints per second.
float Entity::CalculateShieldRegenValue()
{
    float statValue = GetShieldRegenStat();
    float statZeroToOne = MathUtils::RangeMap(statValue, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    statZeroToOne = MathUtils::SmoothStop2(statZeroToOne);
    float adjustedStat = MathUtils::RangeMap(statZeroToOne, 0.0f, 1.0f, Stats::MIN_REGEN_RATE, Stats::MAX_REGEN_RATE);
    return adjustedStat;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateShotDeflectionValue()
{
    float statValue = GetShotDeflectionStat();
    float statZeroToOne = MathUtils::RangeMap(statValue, Stats::MIN_LEVEL, Stats::MAX_LEVEL, 0.0f, 1.0f);
    statZeroToOne = MathUtils::SmoothStop2(statZeroToOne);
    float adjustedStat = MathUtils::RangeMap(statZeroToOne, 0.0f, 1.0f, Stats::MIN_DEFLECTION_VALUE, Stats::MAX_DEFLECTION_VALUE);
    return adjustedStat;
}

//-----------------------------------------------------------------------------------
void Entity::DeleteInventory()
{
    unsigned int inventorySize = m_inventory.size();
    for (unsigned int i = 0; i < inventorySize; ++i)
    {
        if (m_inventory[i])
        {
            delete m_inventory[i];
            m_inventory[i] = nullptr;
        }
    }
}

//-----------------------------------------------------------------------------------
void Entity::DropInventory()
{
    unsigned int inventorySize = m_inventory.size();
    for (unsigned int i = 0; i < inventorySize; ++i)
    {
        //This transfers ownership of the item to the pickup.
        TheGame::instance->m_currentGameMode->SpawnPickup(m_inventory[i], GetPosition());
        m_inventory[i] = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void Entity::SetShieldHealth(float newShieldValue)
{
    newShieldValue = Clamp<float>(newShieldValue, 0.0f, CalculateShieldCapacityValue());
    if (m_currentShieldHealth != newShieldValue)
    {
        m_currentShieldHealth = newShieldValue;
        if (m_currentShieldHealth > 0.0f)
        {
            m_shieldSprite->Enable();
        }
        else
        {
            m_shieldSprite->Disable();
        }
    }
}

//-----------------------------------------------------------------------------------
void Entity::SetVortexShaderPosition(const Vector2& warpHolePosition, int warpHoleID, float vortexRadii)
{
    m_sprite->m_material->SetVec3Uniform(Stringf("gWarpPositions[%i]", warpHoleID).c_str(), Vector3(warpHolePosition, 0.0f));
    m_sprite->m_material->SetFloatUniform(Stringf("gVortexRadii[%i]", warpHoleID).c_str(), vortexRadii);
}

//-----------------------------------------------------------------------------------
void Entity::InitializeInventory(unsigned int inventorySize)
{
    m_inventory.resize(inventorySize);
    for (unsigned int i = 0; i < inventorySize; ++i)
    {
        m_inventory[i] = nullptr;
    }
}