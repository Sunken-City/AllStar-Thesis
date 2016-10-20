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

        Stats& operator+=(const Stats& rhs)
        {
            this->topSpeed += rhs.topSpeed;
            this->acceleration += rhs.acceleration;
            this->agility += rhs.agility;
            this->braking += rhs.braking;
            this->damage += rhs.damage;
            this->shieldDisruption += rhs.shieldDisruption;
            this->shieldPenetration += rhs.shieldPenetration;
            this->rateOfFire += rhs.rateOfFire;
            this->hp += rhs.hp;
            this->shieldCapacity += rhs.shieldCapacity;
            this->shieldRegen += rhs.shieldRegen;
            this->shotDeflection += rhs.shotDeflection;
            return *this;
        }

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
    void AttemptMovement(const Vector2& attemptedPosition);
    void PickUpItem(Item* pickedUpItem);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Weapon* m_weapon;
    Active* m_activeEffect;
    Passive* m_passiveEffect;
    Chassis* m_chassis;
    Stats m_stats;
};