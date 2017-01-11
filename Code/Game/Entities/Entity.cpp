#include "Game/Entities/Entity.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Items/Chassis/Chassis.hpp"
#include "Game/Items/Actives/Actives.hpp"
#include "Game/Items/Passives/Passive.hpp"
#include "Game/StateMachine.hpp"
#include "Game/TheGame.hpp"
#include <algorithm>

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
    , m_baseStats(1.0f)
    , m_velocity(0.0f)
    , m_frictionValue(0.9f)
    , m_collidesWithBullets(true)
    , m_isInvincible(false)
    , m_owner(nullptr)
    , m_noCollide(false)
    , m_shieldValue(0.0f)
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
}

//-----------------------------------------------------------------------------------
void Entity::Update(float deltaSeconds)
{
    m_age += deltaSeconds;
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
void Entity::TakeDamage(float damage)
{
    if (m_isDead || m_isInvincible)
    {
        return;
    }

    m_currentHp -= damage;
    if (m_currentHp < 0.0f)
    {
        Die();
    }
}

//-----------------------------------------------------------------------------------
void Entity::CalculateCollisionRadius()
{
    Vector2 virtualSize = m_sprite->m_spriteResource->m_virtualSize;
    Vector2 spriteScale = m_sprite->m_scale;

    float maxVirtualSize = std::max(virtualSize.x, virtualSize.y);
    float maxSpriteScale = std::max(spriteScale.x, spriteScale.y);
    maxVirtualSize *= 0.5f;
    m_collisionRadius = maxVirtualSize * maxSpriteScale;

    //Get the shield to match the scale of the entity, then add more to encompass it entirely.
    m_shieldSprite->m_scale = Vector2(m_collisionRadius * 2.5f) / (m_shieldSprite->m_spriteResource->m_virtualSize);
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

    m_transform.position = adjustedPosition;
    m_sprite->m_position = adjustedPosition;
    m_shieldSprite->m_position = adjustedPosition;
}

//-----------------------------------------------------------------------------------
void Entity::SetRotation(const float newDegreesRotation)
{
    m_sprite->m_rotationDegrees = newDegreesRotation;
    m_transform.rotationDegrees = newDegreesRotation;
}

//-----------------------------------------------------------------------------------
void Entity::Heal(float healValue)
{
    m_currentHp += healValue;
    MathUtils::Clamp(m_currentHp, 0.0f, GetHpStat());
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
float Entity::GetShieldPenetrationStat()
{
    float shieldPenetration = m_baseStats.shieldPenetration;
    shieldPenetration += m_weapon ? m_weapon->m_statBonuses.shieldPenetration : 0.0f;
    shieldPenetration += m_chassis ? m_chassis->m_statBonuses.shieldPenetration : 0.0f;
    shieldPenetration += m_activeEffect ? m_activeEffect->m_statBonuses.shieldPenetration : 0.0f;
    shieldPenetration += m_passiveEffect ? m_passiveEffect->m_statBonuses.shieldPenetration : 0.0f;
    return shieldPenetration;
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
void Entity::SetShieldValue(float newShieldValue)
{
    if (m_shieldValue != newShieldValue)
    {
        m_shieldValue = newShieldValue;
        if (m_shieldValue > 0.0f)
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