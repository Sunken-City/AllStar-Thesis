#pragma once
#include "Game/Entities/Entity.hpp"

class Sprite;

class Ship : public Entity
{
public:
    Ship();
    virtual ~Ship();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_timeSinceLastShot;
};