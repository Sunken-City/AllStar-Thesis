#include "Game/Entities/Entity.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Items/Chassis/Chassis.hpp"
#include "Game/Items/Actives/Actives.hpp"
#include "Game/Items/Passives/Passive.hpp"

//-----------------------------------------------------------------------------------
Entity::Entity()
    : m_sprite(nullptr)
    , m_hp(1.0f)
    , m_maxHp(1.0f)
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
{

}

//-----------------------------------------------------------------------------------
Entity::~Entity()
{
    if (m_sprite)
    {
        delete m_sprite;
    }
}

//-----------------------------------------------------------------------------------
void Entity::Update(float deltaSeconds)
{
    m_age += deltaSeconds;
}

//-----------------------------------------------------------------------------------
void Entity::Render() const
{

}

//-----------------------------------------------------------------------------------
bool Entity::IsCollidingWith(Entity* otherEntity)
{
    return this->m_sprite->GetBounds().IsIntersecting(otherEntity->m_sprite->GetBounds());
}

//-----------------------------------------------------------------------------------
void Entity::ResolveCollision(Entity* otherEntity)
{
    Vector2& myPosition = this->m_sprite->m_position;
    Vector2 otherPosition = otherEntity->m_sprite->m_position;
    Vector2 difference = myPosition - otherPosition;
    float distanceBetweenPoints = MathUtils::CalcDistanceBetweenPoints(otherPosition, myPosition);
    float firstPushDist = (this->m_collisionRadius - distanceBetweenPoints) / 8.f;
    //float secondPushDist = (otherEntity->m_collisionRadius - distanceBetweenPoints) / 8.f;
    difference *= -firstPushDist;
    myPosition -= difference;
}

//-----------------------------------------------------------------------------------
void Entity::TakeDamage(float damage)
{
    m_hp -= damage;
    if (m_hp < 0.0f)
    {
        m_isDead = true;
        Die();
    }
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
float Entity::GetAgilityStat()
{
    float agility = m_baseStats.agility;
    agility += m_weapon ? m_weapon->m_statBonuses.agility : 0.0f;
    agility += m_chassis ? m_chassis->m_statBonuses.agility : 0.0f;
    agility += m_activeEffect ? m_activeEffect->m_statBonuses.agility : 0.0f;
    agility += m_passiveEffect ? m_passiveEffect->m_statBonuses.agility : 0.0f;
    return agility;
}

//-----------------------------------------------------------------------------------
float Entity::GetWeightStat()
{
    float weight = m_baseStats.weight;
    weight += m_weapon ? m_weapon->m_statBonuses.weight : 0.0f;
    weight += m_chassis ? m_chassis->m_statBonuses.weight : 0.0f;
    weight += m_activeEffect ? m_activeEffect->m_statBonuses.weight : 0.0f;
    weight += m_passiveEffect ? m_passiveEffect->m_statBonuses.weight : 0.0f;
    return weight;
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

