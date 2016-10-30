#pragma once
#include "Engine/Math/Transform2D.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/Stats.hpp"

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
    virtual bool IsCollidingWith(Entity* otherEntity);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void TakeDamage(float damage);
    virtual void Die() { m_isDead = true; };
    inline virtual bool IsPlayer() { return false; }
    virtual void SetPosition(const Vector2& newPosition);
    virtual Vector2 GetPosition() { return m_transform.position; };
    virtual void Heal(float healValue);

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

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Weapon* m_weapon;
    Active* m_activeEffect;
    Passive* m_passiveEffect;
    Chassis* m_chassis;
    Stats m_baseStats;

    Sprite* m_sprite;
    Transform2D m_transform;
    Vector2 m_velocity;
    float m_currentHp;
    float m_collisionRadius;
    float m_age;
    float m_frictionValue;
    bool m_isDead;
};