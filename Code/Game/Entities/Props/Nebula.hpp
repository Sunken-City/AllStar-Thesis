#pragma once
#include "Game/Entities/Entity.hpp"

class Nebula : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Nebula(const Vector2& position);
    virtual ~Nebula();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    inline virtual bool IsProp() override { return true; };
    inline virtual bool ShowsDamageNumbers() { return false; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
};