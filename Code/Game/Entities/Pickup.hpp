#pragma once
#include "Game/Entities/Ship.hpp"

enum PickupType
{
    SPEED = 0,
    POWER,
    DEFENCE,
    FIRERATE,
    HP,
    NUM_TYPES
};

class Pickup : public Entity
{
public:
    Pickup(const Vector2& position);
    virtual ~Pickup();

    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    PickupType m_type;

    //CONSTANTS/////////////////////////////////////////////////////////////////////
    static const float MAX_ANGULAR_VELOCITY;
};