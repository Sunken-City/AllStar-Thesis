#pragma once
#include "Game/Entities/Projectiles/Projectile.hpp"

class Laser : public Projectile
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Laser(Entity* owner, float degreesOffset = 0.0f, float damage = 1.0f, float disruption = 0.0f, float homing = 0.0f);
    virtual ~Laser();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static const float KNOCKBACK_MAGNITUDE;

    Vector2 m_muzzleDirection = Vector2::ZERO;
};
