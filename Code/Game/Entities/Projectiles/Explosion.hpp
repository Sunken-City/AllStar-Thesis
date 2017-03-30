#pragma once
#include "Game/Entities/Projectiles/Projectile.hpp"

class Explosion : public Projectile
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Explosion(Entity* owner, const Vector2& spawnPosition, float damage = 1.0f, float disruption = 0.0f);
    virtual ~Explosion();
    virtual void Update(float deltaSeconds) override;
    virtual float GetKnockbackMagnitude() override;
    virtual void ResolveCollision(Entity* otherEntity) override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static const float KNOCKBACK_MAGNITUDE;
    static constexpr float MAX_SCALE = 0.5f;

    Vector2 m_muzzleDirection = Vector2::ZERO;
};
