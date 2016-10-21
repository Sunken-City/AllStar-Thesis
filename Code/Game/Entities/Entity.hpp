#pragma once
#include "Engine/Math/Transform2D.hpp"

class Sprite;

class Entity
{
public:
    Entity();
    virtual ~Entity();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual bool IsCollidingWith(Entity* otherEntity);
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void TakeDamage(float m_power);
    virtual void Die() {};

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Sprite* m_sprite;
    Transform2D m_transform;
    float m_hp;
    float m_maxHp;
    float m_collisionRadius;
    float m_age;
    bool m_isDead;
};