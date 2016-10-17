#pragma once
#include "Engine/Components/Transform3D.hpp"

class Sprite;

class Entity
{
public:
    Entity();
    virtual ~Entity();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual bool IsCollidingWith(Entity* otherEntity);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void TakeDamage(float m_power);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Sprite* m_sprite;
    float m_defence;
    float m_hp;
    float m_maxHp;
    float m_collisionRadius;
    float m_age;
    bool m_isDead;
};