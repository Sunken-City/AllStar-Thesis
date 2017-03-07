#pragma once
#include "Game/Entities/Projectiles/Projectile.hpp"

class Laser : public Projectile
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
    Laser(Entity* owner, float degreesOffset = 0.0f, float damage = 1.0f, float disruption = 0.0f, float homing = 0.0f, MovementBehavior behavior = STRAIGHT);
    virtual ~Laser();
    virtual void Update(float deltaSeconds);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    MovementBehavior m_behavior = STRAIGHT;
    Vector2 m_muzzleDirection = Vector2::ZERO;
};
