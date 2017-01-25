#include "Game/Entities/Entity.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Items/Chassis/Chassis.hpp"
#include "Game/Items/Actives/Actives.hpp"
#include "Game/Items/Passives/Passive.hpp"
#include "Game/StateMachine.hpp"
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"
#include <algorithm>
#include "TextSplash.hpp"

Vector2 Entity::SHEILD_SCALE_FUDGE_VALUE = Vector2(0.25f);

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
    , m_baseStats(0.0f)
    , m_velocity(0.0f)
    , m_frictionValue(0.9f)
    , m_collidesWithBullets(true)
    , m_isInvincible(false)
    , m_owner(nullptr)
    , m_noCollide(false)
    , m_currentShieldHealth(0.0f)
{
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
    if (m_isDead || otherEntity->m_isDead || otherEntity->m_noCollide || otherEntity == m_owner || otherEntity->m_owner == this || (!m_collidesWithBullets && (otherEntity->IsProjectile() || IsProjectile())))
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
    SetPosition(myPosition + myPositionCorrection);
    otherEntity->SetPosition(otherPosition - myPositionCorrection);
}

//-----------------------------------------------------------------------------------
float Entity::TakeDamage(float damage, float disruption /*= 1.0f*/)
{
    if (m_isDead || m_isInvincible)
    {
        return -1.0f;
    }

    float damageDealt = 0.0f;
    float randomPercentage = MathUtils::GetRandomFloatFromZeroTo(1.0f) - 0.5f;
    damage += damage * randomPercentage;
    float randomDegrees = MathUtils::GetRandom(-90.0f, 90.0f);
    Vector2 velocity = Vector2::DegreesToDirection(randomDegrees, Vector2::ZERO_DEGREES_UP) * 2.0f;

    if (HasShield())
    {
        float adjustedDamage = damage + (damage * disruption);
        SetShieldHealth(m_currentShieldHealth - adjustedDamage);
        damageDealt = adjustedDamage;
        TextSplash::CreateTextSplash(Stringf("%i", (int)(damageDealt * 10.0f)), m_transform, velocity, RGBA(0.0f, 0.5 + randomPercentage, 1.0f, 1.0f));
    }
    else
    {
        m_currentHp -= damage;
        damageDealt = damage;
        TextSplash::CreateTextSplash(Stringf("%i", (int)(damageDealt * 10.0f)), m_transform, velocity, RGBA(1.0f, 1.0f - (0.5 + randomPercentage), 0.0f, 1.0f));
        if (m_currentHp <= 0.0f)
        {
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
    Vector2 spriteScale = m_sprite->m_transform.GetWorldScale();;

    float maxVirtualSize = Max(virtualSize.x, virtualSize.y);
    float maxSpriteScale = Max(spriteScale.x, spriteScale.y);
    maxVirtualSize *= 0.5f;
    m_collisionRadius = maxVirtualSize * maxSpriteScale;

    //Get the shield to match the scale of the entity, then add more to encompass it entirely.
    m_shieldSprite->m_transform.SetScale(Vector2(m_collisionRadius * 2.5f) / (m_shieldSprite->m_spriteResource->m_virtualSize));
    //m_shieldSprite->m_scale += SHEILD_SCALE_FUDGE_VALUE;
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
    m_sprite->m_transform.SetPosition(adjustedPosition);
    m_shieldSprite->m_transform.SetPosition(adjustedPosition);
}

//-----------------------------------------------------------------------------------
void Entity::SetRotation(const float newDegreesRotation)
{
    m_sprite->m_transform.SetRotationDegrees(newDegreesRotation);
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
    return topSpeed;
}

//-----------------------------------------------------------------------------------
float Entity::GetAccelerationStat()
{
    float totalAcceleration = m_baseStats.acceleration;
    totalAcceleration += m_weapon ? m_weapon->m_statBonuses.acceleration : 0.0f;
    totalAcceleration += m_chassis ? m_chassis->m_statBonuses.acceleration : 0.0f;
    totalAcceleration += m_activeEffect ? m_activeEffect->m_statBonuses.acceleration : 0.0f;
    totalAcceleration += m_passiveEffect ? m_passiveEffect->m_statBonuses.acceleration : 0.0f;
    return totalAcceleration;
}

//-----------------------------------------------------------------------------------
float Entity::GetHandlingStat()
{
    float handling = m_baseStats.handling;
    handling += m_weapon ? m_weapon->m_statBonuses.handling : 0.0f;
    handling += m_chassis ? m_chassis->m_statBonuses.handling : 0.0f;
    handling += m_activeEffect ? m_activeEffect->m_statBonuses.handling : 0.0f;
    handling += m_passiveEffect ? m_passiveEffect->m_statBonuses.handling : 0.0f;
    return handling;
}

//-----------------------------------------------------------------------------------
float Entity::GetBrakingStat()
{
    float braking = m_baseStats.braking;
    braking += m_weapon ? m_weapon->m_statBonuses.braking : 0.0f;
    braking += m_chassis ? m_chassis->m_statBonuses.braking : 0.0f;
    braking += m_activeEffect ? m_activeEffect->m_statBonuses.braking : 0.0f;
    braking += m_passiveEffect ? m_passiveEffect->m_statBonuses.braking : 0.0f;
    return braking;
}

//-----------------------------------------------------------------------------------
float Entity::GetDamageStat()
{
    float damage = m_baseStats.damage;
    damage += m_weapon ? m_weapon->m_statBonuses.damage : 0.0f;
    damage += m_chassis ? m_chassis->m_statBonuses.damage : 0.0f;
    damage += m_activeEffect ? m_activeEffect->m_statBonuses.damage : 0.0f;
    damage += m_passiveEffect ? m_passiveEffect->m_statBonuses.damage : 0.0f;
    return damage;
}

//-----------------------------------------------------------------------------------
float Entity::GetShieldDisruptionStat()
{
    float shieldDisruption = m_baseStats.shieldDisruption;
    shieldDisruption += m_weapon ? m_weapon->m_statBonuses.shieldDisruption : 0.0f;
    shieldDisruption += m_chassis ? m_chassis->m_statBonuses.shieldDisruption : 0.0f;
    shieldDisruption += m_activeEffect ? m_activeEffect->m_statBonuses.shieldDisruption : 0.0f;
    shieldDisruption += m_passiveEffect ? m_passiveEffect->m_statBonuses.shieldDisruption : 0.0f;
    return shieldDisruption;
}

//-----------------------------------------------------------------------------------
float Entity::GetShotHomingStat()
{
    float shotHoming = m_baseStats.shotHoming;
    shotHoming += m_weapon ? m_weapon->m_statBonuses.shotHoming : 0.0f;
    shotHoming += m_chassis ? m_chassis->m_statBonuses.shotHoming : 0.0f;
    shotHoming += m_activeEffect ? m_activeEffect->m_statBonuses.shotHoming : 0.0f;
    shotHoming += m_passiveEffect ? m_passiveEffect->m_statBonuses.shotHoming : 0.0f;
    return shotHoming;
}

//-----------------------------------------------------------------------------------
float Entity::GetRateOfFireStat()
{
    float rateOfFire = m_baseStats.rateOfFire;
    rateOfFire += m_weapon ? m_weapon->m_statBonuses.rateOfFire : 0.0f;
    rateOfFire += m_chassis ? m_chassis->m_statBonuses.rateOfFire : 0.0f;
    rateOfFire += m_activeEffect ? m_activeEffect->m_statBonuses.rateOfFire : 0.0f;
    rateOfFire += m_passiveEffect ? m_passiveEffect->m_statBonuses.rateOfFire : 0.0f;
    return rateOfFire;
}

//-----------------------------------------------------------------------------------
float Entity::GetHpStat()
{
    float hp = m_baseStats.hp;
    hp += m_weapon ? m_weapon->m_statBonuses.hp : 0.0f;
    hp += m_chassis ? m_chassis->m_statBonuses.hp : 0.0f;
    hp += m_activeEffect ? m_activeEffect->m_statBonuses.hp : 0.0f;
    hp += m_passiveEffect ? m_passiveEffect->m_statBonuses.hp : 0.0f;
    return hp;
}

//-----------------------------------------------------------------------------------
float Entity::GetShieldCapacityStat()
{
    float shieldCapacity = m_baseStats.shieldCapacity;
    shieldCapacity += m_weapon ? m_weapon->m_statBonuses.shieldCapacity : 0.0f;
    shieldCapacity += m_chassis ? m_chassis->m_statBonuses.shieldCapacity : 0.0f;
    shieldCapacity += m_activeEffect ? m_activeEffect->m_statBonuses.shieldCapacity : 0.0f;
    shieldCapacity += m_passiveEffect ? m_passiveEffect->m_statBonuses.shieldCapacity : 0.0f;
    return shieldCapacity;
}

//-----------------------------------------------------------------------------------
float Entity::GetShieldRegenStat()
{
    float shieldRegen = m_baseStats.shieldRegen;
    shieldRegen += m_weapon ? m_weapon->m_statBonuses.shieldRegen : 0.0f;
    shieldRegen += m_chassis ? m_chassis->m_statBonuses.shieldRegen : 0.0f;
    shieldRegen += m_activeEffect ? m_activeEffect->m_statBonuses.shieldRegen : 0.0f;
    shieldRegen += m_passiveEffect ? m_passiveEffect->m_statBonuses.shieldRegen : 0.0f;
    return shieldRegen;
}

//-----------------------------------------------------------------------------------
float Entity::GetShotDeflectionStat()
{
    float shotDeflection = m_baseStats.shotDeflection;
    shotDeflection += m_weapon ? m_weapon->m_statBonuses.shotDeflection : 0.0f;
    shotDeflection += m_chassis ? m_chassis->m_statBonuses.shotDeflection : 0.0f;
    shotDeflection += m_activeEffect ? m_activeEffect->m_statBonuses.shotDeflection : 0.0f;
    shotDeflection += m_passiveEffect ? m_passiveEffect->m_statBonuses.shotDeflection : 0.0f;
    return shotDeflection;
}

//-----------------------------------------------------------------------------------
float Entity::CalculateTopSpeedValue()
{
    return Stats::BASE_SPEED_VALUE + (GetTopSpeedStat() * Stats::SPEED_VALUE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateAccelerationValue()
{
    return Stats::BASE_ACCELERATION_VALUE + (GetAccelerationStat() * Stats::ACCELERATION_VALUE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateHandlingValue()
{
    return Stats::BASE_HANDLING_VALUE + (GetHandlingStat() * Stats::HANDLING_VALUE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateBrakingValue()
{
    return Stats::BASE_BRAKING_VALUE + (GetBrakingStat() * Stats::BRAKING_VALUE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateDamageValue()
{
    return Stats::BASE_DAMAGE_VALUE + (GetDamageStat() * Stats::DAMAGE_VALUE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateShieldDisruptionValue()
{
    return Stats::BASE_DISRUPTION_PERCENTAGE + (GetShieldDisruptionStat() * Stats::DISRUPTION_PERCENTAGE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateShotHomingValue()
{
    return Stats::BASE_SHOT_HOMING_VALUE + (GetShotHomingStat() * Stats::SHOT_HOMING_VALUE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateRateOfFireValue()
{
    return Stats::BASE_RATE_OF_FIRE + (GetRateOfFireStat() * Stats::RATE_OF_FIRE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateHpValue()
{
    return Stats::BASE_HP_VALUE + (GetHpStat() * Stats::HP_VALUE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateShieldCapacityValue()
{
    return Stats::BASE_CAPACITY_VALUE + (GetShieldCapacityStat() * Stats::CAPACITY_VALUE_PER_POINT);
}

//-----------------------------------------------------------------------------------
//This metric is in shield capacity points per second.
float Entity::CalculateShieldRegenValue()
{
    return Stats::BASE_REGEN_RATE + (GetShieldRegenStat() * Stats::REGEN_RATE_PER_POINT);
}

//-----------------------------------------------------------------------------------
float Entity::CalculateShotDeflectionValue()
{
    return Stats::BASE_DEFLECTION_VALUE + (GetShotDeflectionStat() * Stats::DEFLECTION_VALUE_PER_POINT);
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
void Entity::InitializeInventory(unsigned int inventorySize)
{
    m_inventory.resize(inventorySize);
    for (unsigned int i = 0; i < inventorySize; ++i)
    {
        m_inventory[i] = nullptr;
    }
}