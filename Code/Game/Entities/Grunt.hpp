#pragma once
#include "Game/Entities/Ship.hpp"

class Grunt : public Ship
{
public:
    Grunt(const Vector2& position);
    virtual ~Grunt();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void Die();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
};