#pragma once
#include "Game/Entities/Ship.hpp"
#include "Game/Stats.hpp"
#include <stdint.h>

enum class PowerUpType;

class PlayerShip : public Ship
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    PlayerShip();
    ~PlayerShip();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds);
    virtual void Render() const;
    virtual void ResolveCollision(Entity* otherEntity);
    virtual void Die();

    void DropPowerups();
    void AttemptMovement(const Vector2& attemptedPosition);
    void PickUpItem(Item* pickedUpItem);
    void DropRandomPowerup();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Stats m_powerupStatModifiers;
};