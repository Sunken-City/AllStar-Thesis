#pragma once
#include "Game/Entities/Entity.hpp"

class Projectile : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Projectile(Entity* owner, float degreesOffset = 0.0f, float power = 1.0f, float disruption = 0.0f, float homing = 0.0f);
    virtual ~Projectile();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual inline bool IsProjectile() { return true; };
    void ApplyImpulse(const Vector2& appliedAcceleration);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Vector2 m_sumOfImpulses = Vector2::ZERO;
    float m_speed;
    float m_power;
    float m_disruption;
    float m_shotHoming;
    float m_lifeSpan;
};