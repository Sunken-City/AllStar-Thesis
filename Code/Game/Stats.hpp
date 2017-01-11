#pragma once

enum class PowerUpType;

struct Stats
{
    Stats(float defaultValue = 0.0f);

    unsigned int GetTotalNumberOfDroppablePowerUps();
    float* GetStatReference(PowerUpType type);
    Stats& operator+=(const Stats& rhs);

    float topSpeed;
    float acceleration;
    float handling;
    float braking;
    float damage;
    float shieldDisruption;
    float shieldPenetration;
    float rateOfFire;
    float hp;
    float shieldCapacity;
    float shieldRegen;
    float shotDeflection;

    //STATIC VARIABLES/////////////////////////////////////////////////////////////////////
    static const float HP_VALUE_PER_POINT;
    static const float CAPACITY_VALUE_PER_POINT;
};