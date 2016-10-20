#pragma once
#include "Game/Entities/Entity.hpp"

class Bullet : public Entity
{
public:
    Bullet(Entity* Owner);
    virtual ~Bullet();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_speed;
    float m_power;
    float m_lifeSpan;
    Entity* m_owner;
};