#pragma once
#include "Game/Entities/Ship.hpp"
#include <stdint.h>

class Player : public Ship
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Player();
    ~Player();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity);
    void AttemptMovement(const Vector2& attemptedPosition);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};