#pragma once
#include "Game/Entities/Entity.hpp"

class Projectile : public Entity
{
public:
    Projectile(Entity* Owner, float power = 1.0f, float disruption = 1.0f, float penetration = 0.0f);
    virtual ~Projectile();

    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual inline bool IsProjectile() { return true; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_speed;
    float m_power;
    float m_disruption;
    float m_penetration;
    float m_lifeSpan;
};