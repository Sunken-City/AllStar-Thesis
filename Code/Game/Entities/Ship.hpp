#pragma once
#include "Game/Entities/Entity.hpp"

class Sprite;
class Pilot;
class Vector2;

class Ship : public Entity
{
public:
    Ship(Pilot* pilot = nullptr);
    virtual ~Ship();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity);
    void AttemptMovement(const Vector2& attemptedPosition);
    void UpdateMotion(float deltaSeconds);
    void UpdateShooting();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_timeSinceLastShot;
    Pilot* m_pilot;
};