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
    float shotHoming;
    float rateOfFire;
    float hp;
    float shieldCapacity;
    float shieldRegen;
    float shotDeflection;

    //STATIC VARIABLES/////////////////////////////////////////////////////////////////////
    static const float MIN_LEVEL;
    static const float MAX_LEVEL;

    //Minimum Stat Values
    static const float MIN_SPEED_VALUE;
    static const float MIN_ACCELERATION_VALUE;
    static const float MIN_HANDLING_VALUE;
    static const float MIN_BRAKING_VALUE;
    static const float MIN_DAMAGE_VALUE;
    static const float MIN_DISRUPTION_PERCENTAGE;
    static const float MIN_SHOT_HOMING_VALUE;
    static const float MIN_RATE_OF_FIRE;
    static const float MIN_HP_VALUE;
    static const float MIN_CAPACITY_VALUE;
    static const float MIN_REGEN_RATE;
    static const float MIN_DEFLECTION_VALUE;

    //Maximum Stat Values
    static const float MAX_SPEED_VALUE;
    static const float MAX_ACCELERATION_VALUE;
    static const float MAX_HANDLING_VALUE;
    static const float MAX_BRAKING_VALUE;
    static const float MAX_DAMAGE_VALUE;
    static const float MAX_DISRUPTION_PERCENTAGE;
    static const float MAX_SHOT_HOMING_VALUE;
    static const float MAX_RATE_OF_FIRE;
    static const float MAX_HP_VALUE;
    static const float MAX_CAPACITY_VALUE;
    static const float MAX_REGEN_RATE;
    static const float MAX_DEFLECTION_VALUE;

};