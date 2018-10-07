#pragma once
#include "Game/Entities/Ship.hpp"

class Brute : public Ship
{
public:
    Brute(const Vector2& position);
    virtual ~Brute();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void Die();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
};