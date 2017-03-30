#pragma once
#include "Game/Entities/Projectiles/Projectile.hpp"

class RibbonParticleSystem;

class Missile : public Projectile
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Missile(Entity* owner, float degreesOffset = 0.0f, float power = 1.0f, float disruption = 0.0f, float homing = 0.0f);
    virtual ~Missile();
    virtual bool FlushParticleTrailIfExists();
    virtual float GetKnockbackMagnitude() override;
    virtual void LockOn() override;
    virtual void Update(float deltaSeconds) override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static const float KNOCKBACK_MAGNITUDE;
    RibbonParticleSystem* m_missileTrail;
    bool m_hasLockedOn = false;
};