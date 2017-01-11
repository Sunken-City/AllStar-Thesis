#pragma once
#include "Engine/Math/Transform2D.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/Stats.hpp"
#include <vector>

class Sprite;
class Weapon;
class Active;
class Passive;
class Chassis;
class Item;

class Entity
{
public:
    Entity();
    virtual ~Entity();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void TakeDamage(float damage);
    virtual void Die() { m_isDead = true; };
    virtual void CalculateCollisionRadius();
    virtual void SetPosition(const Vector2& newPosition);
    virtual void SetRotation(const float newDegreesRotation);
    virtual void Heal(float healValue);
    virtual void DropInventory();
    virtual void SetShieldValue(float newShieldValue);
    void InitializeInventory(unsigned int inventorySize);
    void DeleteInventory();

    //QUERIES/////////////////////////////////////////////////////////////////////
    inline virtual bool IsPlayer() { return false; };
    inline virtual bool HasShield() { return m_shieldHealth > 0.0f; };
    inline virtual Vector2 GetPosition() { return m_transform.position; };
    inline virtual float GetRotation() { return m_transform.rotationDegrees; };
    virtual bool IsCollidingWith(Entity* otherEntity);

    //STAT FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual float GetTopSpeedStat();
    virtual float GetAccelerationStat();
    virtual float GetHandlingStat();
    virtual float GetBrakingStat();
    virtual float GetDamageStat();
    virtual float GetShieldDisruptionStat();
    virtual float GetShieldPenetrationStat();
    virtual float GetRateOfFireStat();
    virtual float GetHpStat();
    virtual float GetShieldCapacityStat();
    virtual float GetShieldRegenStat();
    virtual float GetShotDeflectionStat();
    virtual inline bool IsProjectile() { return false; };

    //STATIC VARIABLES/////////////////////////////////////////////////////////////////////
    static Vector2 SHEILD_SCALE_FUDGE_VALUE;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Weapon* m_weapon;
    Active* m_activeEffect;
    Passive* m_passiveEffect;
    Chassis* m_chassis;
    Stats m_baseStats;

    Sprite* m_sprite;
    Sprite* m_shieldSprite;
    Entity* m_owner;
    Transform2D m_transform;
    Vector2 m_velocity;
    std::vector<Item*> m_inventory;
    float m_currentHp;
    float m_collisionRadius;
    float m_age;
    float m_frictionValue;
    float m_shieldHealth;
    bool m_isDead;
    bool m_collidesWithBullets;
    bool m_noCollide;
    bool m_isInvincible;
    bool m_staysWithinBounds;
};