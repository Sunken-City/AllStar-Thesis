#include "Game/Entities/Entity.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Items/Chassis/Chassis.hpp"
#include "Game/Items/Actives/Actives.hpp"
#include "Game/Items/Passives/Passive.hpp"
#include "Game/StateMachine.hpp"
#include "Game/TheGame.hpp"

//-----------------------------------------------------------------------------------
Entity::Entity()
    : m_sprite(nullptr)
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
{

}

//-----------------------------------------------------------------------------------
Entity::~Entity()
{
    if (m_sprite)
    {
        delete m_sprite;
    }
    if (GetGameState() == GameState::PLAYING)
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
    return this->m_sprite->GetBounds().IsIntersecting(otherEntity->m_sprite->GetBounds());
}

//-----------------------------------------------------------------------------------
void Entity::ResolveCollision(Entity* otherEntity)
{
    if (m_isDead || otherEntity->m_isDead || (!m_collidesWithBullets && (otherEntity->IsProjectile() || IsProjectile())))
    {
        return;
    }
    Vector2 myPosition = GetPosition();
    Vector2 otherPosition = otherEntity->GetPosition();
    Vector2 difference = myPosition - otherPosition;
    float distanceBetweenPoints = MathUtils::CalcDistanceBetweenPoints(otherPosition, myPosition);
    float pushDistance = (this->m_collisionRadius - distanceBetweenPoints) / 8.f;
    difference *= -pushDistance;
    SetPosition(myPosition - difference);
    otherEntity->SetPosition(otherPosition + difference);

    /*
    Vector2 myPosition = GetPosition();
    Vector2 otherPosition = otherEntity->GetPosition();
    Vector2 distanceFromOtherToMe = myPosition - otherPosition;
    Vector2 vectorFromOtherToMe = distanceFromOtherToMe.GetNorm();

    float distanceBetweenEntities = MathUtils::CalcDistanceBetweenPoints(otherPosition, myPosition);
    float pushScale = (this->m_collisionRadius - distanceBetweenEntities) / 2.f;
    Vector2 collisionDisplacementVector = vectorFromOtherToMe * -pushScale;
    SetPosition(myPosition - collisionDisplacementVector);
    otherEntity->SetPosition(otherPosition + collisionDisplacementVector);
    */
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
        m_isDead = true;
        Die();
    }
}

//-----------------------------------------------------------------------------------
void Entity::SetPosition(const Vector2& newPosition)
{
    m_transform.position = newPosition;
    m_sprite->m_position = newPosition;
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
        TheGame::instance->SpawnPickup(m_inventory[i], GetPosition());
        m_inventory[i] = nullptr;
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