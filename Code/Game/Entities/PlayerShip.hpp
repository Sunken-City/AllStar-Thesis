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

    //STAT FUNCTIONS/////////////////////////////////////////////////////////////////////
    inline virtual float GetTopSpeed() { return Ship::GetTopSpeed() + m_powerupStatModifiers.topSpeed; };
    inline virtual float GetAcceleration() { return Ship::GetAcceleration() + m_powerupStatModifiers.acceleration; };
    inline virtual float GetAgility() { return Ship::GetAgility() + m_powerupStatModifiers.agility; };
    inline virtual float GetBraking() { return Ship::GetBraking() + m_powerupStatModifiers.braking; };
    inline virtual float GetDamage() { return Ship::GetDamage() + m_powerupStatModifiers.damage; };
    inline virtual float GetShieldDisruption() { return Ship::GetShieldDisruption() + m_powerupStatModifiers.shieldDisruption; };
    inline virtual float GetShieldPenetration() { return Ship::GetShieldPenetration() + m_powerupStatModifiers.shieldPenetration; };
    inline virtual float GetRateOfFire() { return Ship::GetRateOfFire() + m_powerupStatModifiers.rateOfFire; };
    inline virtual float GetHp() { return Ship::GetHp() + m_powerupStatModifiers.hp; };
    inline virtual float GetShieldCapacity() { return Ship::GetShieldCapacity() + m_powerupStatModifiers.shieldCapacity; };
    inline virtual float GetShieldRegen() { return Ship::GetShieldRegen() + m_powerupStatModifiers.shieldRegen; };
    inline virtual float GetShotDeflection() { return Ship::GetShotDeflection() + m_powerupStatModifiers.shotDeflection; };

    void DropPowerups();
    void AttemptMovement(const Vector2& attemptedPosition);
    void PickUpItem(Item* pickedUpItem);
    void DropRandomPowerup();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Stats m_powerupStatModifiers;
};