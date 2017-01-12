#pragma once
#include "Game/Entities/Entity.hpp"

class Projectile : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Projectile(Entity* Owner, float power = 1.0f, float disruption = 0.0f, float penetration = 0.0f);
    virtual ~Projectile();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual inline bool IsProjectile() { return true; };
    void ApplyImpulse(const Vector2& appliedAcceleration);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Vector2 m_accelerationViaImpulse = Vector2::ZERO;
    float m_speed;
    float m_power;
    float m_disruption;
    float m_penetration;
    float m_lifeSpan;
};