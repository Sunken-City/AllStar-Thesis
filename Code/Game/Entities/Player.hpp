#pragma once
#include "Game/Entities/Ship.hpp"
#include <stdint.h>

class Weapon;
class Active;
class Passive;
class Chassis;
class Item;
enum class PowerUpType;

class Player : public Ship
{
public:
    struct Stats
    {
        Stats();

        unsigned int GetTotalNumberOfDroppablePowerUps();
        short* GetStatReference(PowerUpType type);
        Stats& operator+=(const Stats& rhs);

        short topSpeed;
        short acceleration;
        short agility;
        short braking;
        short damage;
        short shieldDisruption;
        short shieldPenetration;
        short rateOfFire;
        short hp;
        short shieldCapacity;
        short shieldRegen;
        short shotDeflection;
    };

    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Player();
    ~Player();

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
    Weapon* m_weapon;
    Active* m_activeEffect;
    Passive* m_passiveEffect;
    Chassis* m_chassis;
    Stats m_stats;
};