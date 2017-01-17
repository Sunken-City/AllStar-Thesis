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
    //Base Stats
    static const float BASE_SPEED_VALUE;
    static const float BASE_ACCELERATION_VALUE;
    static const float BASE_HANDLING_VALUE;
    static const float BASE_BRAKING_VALUE;
    static const float BASE_DAMAGE_VALUE;
    static const float BASE_DISRUPTION_PERCENTAGE;
    static const float BASE_PENETRATION_PERCENTAGE;
    static const float BASE_RATE_OF_FIRE;
    static const float BASE_HP_VALUE;
    static const float BASE_CAPACITY_VALUE;
    static const float BASE_REGEN_RATE;
    static const float BASE_DEFLECTION_VALUE;

    //Value per Pickup/Stat point
    static const float SPEED_VALUE_PER_POINT;
    static const float ACCELERATION_VALUE_PER_POINT;
    static const float HANDLING_VALUE_PER_POINT;
    static const float BRAKING_VALUE_PER_POINT;
    static const float DAMAGE_VALUE_PER_POINT;
    static const float DISRUPTION_PERCENTAGE_PER_POINT;
    static const float PENETRATION_PERCENTAGE_PER_POINT;
    static const float RATE_OF_FIRE_PER_POINT;
    static const float HP_VALUE_PER_POINT;
    static const float CAPACITY_VALUE_PER_POINT;
    static const float REGEN_RATE_PER_POINT;
    static const float DEFLECTION_VALUE_PER_POINT;

};