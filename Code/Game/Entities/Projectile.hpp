#pragma once
#include "Game/Entities/Entity.hpp"

class Projectile : public Entity
{
public:
    Projectile(Entity* Owner);
    virtual ~Projectile();

    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual inline bool IsProjectile() { return true; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_speed;
    float m_power;
    float m_lifeSpan;
    Entity* m_owner;
};