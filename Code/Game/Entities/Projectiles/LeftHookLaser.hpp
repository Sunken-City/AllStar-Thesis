#pragma once
#include "Game/Entities/Projectiles/Projectile.hpp"

class LeftHookLaser : public Projectile
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    LeftHookLaser(Entity* owner, float degreesOffset = 0.0f, float damage = 1.0f, float disruption = 0.0f, float homing = 0.0f);
    virtual ~LeftHookLaser();
    virtual float GetKnockbackMagnitude() override;
    virtual void Update(float deltaSeconds) override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static const float KNOCKBACK_MAGNITUDE;

    Vector2 m_muzzleDirection = Vector2::ZERO;
    Vector2 m_spawnPosition = Vector2::ZERO;
    float m_lastLifespan = 0.0f;
};
