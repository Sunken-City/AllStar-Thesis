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
    virtual void Render() const;
    virtual bool IsCollidingWith(Entity* otherEntity);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void TakeDamage(float m_power);
    virtual void Die() {};

    //STAT FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual float GetTopSpeedStat();
    virtual float GetAccelerationStat();
    virtual float GetAgilityStat();
    virtual float GetWeightStat();
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
    float m_hp;
    float m_maxHp;
    float m_collisionRadius;
    float m_age;
    float m_frictionValue;
    bool m_isDead;
};