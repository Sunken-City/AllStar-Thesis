#pragma once
#include "Game/Entities/Entity.hpp"

class HealingZone : public Entity
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    HealingZone(const Vector2& position);
    virtual ~HealingZone();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void ResolveCollision(Entity* otherEntity) override;
    inline virtual bool IsProp() override { return true; };
    inline virtual bool ShowsDamageNumbers() { return false; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    float m_angularVelocity;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
};