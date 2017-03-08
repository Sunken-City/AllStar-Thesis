#pragma once
#include "Game/Entities/Projectiles/Projectile.hpp"

class RibbonParticleSystem;

class PlasmaBall : public Projectile
{
public:

    enum MovementBehavior
    {
        STRAIGHT = 0,
        LEFT_WAVE,
        RIGHT_WAVE,
        NUM_BEHAVIORS
    };

    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    PlasmaBall(Entity* owner, float degreesOffset = 0.0f, float damage = 1.0f, float disruption = 0.0f, float homing = 0.0f, MovementBehavior behavior = STRAIGHT);
    virtual ~PlasmaBall();
    virtual void Update(float deltaSeconds) override;
    virtual bool FlushParticleTrailIfExists() override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    MovementBehavior m_behavior = STRAIGHT;
    Vector2 m_muzzleDirection = Vector2::ZERO;
    RibbonParticleSystem* m_trail = nullptr;
};
