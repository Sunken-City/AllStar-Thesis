#pragma once
#include "Game/Entities/Ship.hpp"
#include <stdint.h>

class Weapon;
class Active;
class Passive;
class Chassis;

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
    Weapon* m_weapon;
    Active* m_activeEffect;
    Passive* m_passiveEffect;
    Chassis* m_chassis;
};