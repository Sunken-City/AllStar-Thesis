#pragma once
#include "Game/Entities/Entity.hpp"

class Asteroid : public Entity
{
public:
    Asteroid(const Vector2& position);
    virtual ~Asteroid();

    virtual void Die() override;
    virtual void Update(float deltaSeconds);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
    static const float MIN_ASTEROID_SCALE;
    static const float MAX_ASTEROID_SCALE;
};