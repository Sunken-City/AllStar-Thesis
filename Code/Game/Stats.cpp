#include "Game/Stats.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const float Stats::MIN_LEVEL = 1.0f;
const float Stats::MAX_LEVEL = 36.0f;

//SPEED/////////////////////////////////////////////////////////////////////
const float Stats::MIN_SPEED_VALUE = 3.0f;
const float Stats::MAX_SPEED_VALUE = 20.0f;
const float Stats::MIN_ACCELERATION_VALUE = 0.05f;
const float Stats::MAX_ACCELERATION_VALUE = 0.9f;
const float Stats::MIN_HANDLING_VALUE = 0.05f;
const float Stats::MAX_HANDLING_VALUE = 0.9f;
const float Stats::MIN_BRAKING_VALUE = 0.98f;
const float Stats::MAX_BRAKING_VALUE = 0.95f;

//POWER/////////////////////////////////////////////////////////////////////
const float Stats::MIN_DAMAGE_VALUE = 2.0f;
const float Stats::MAX_DAMAGE_VALUE = 50.0f;
const float Stats::MIN_DISRUPTION_PERCENTAGE = 0.0f;
const float Stats::MAX_DISRUPTION_PERCENTAGE = 0.5f;
const float Stats::MIN_SHOT_HOMING_VALUE = 0.0f;
const float Stats::MAX_SHOT_HOMING_VALUE = 30.0f;
const float Stats::MIN_RATE_OF_FIRE = 3.0f; //Shots per second
const float Stats::MAX_RATE_OF_FIRE = 10.0f;

//DEFENSE/////////////////////////////////////////////////////////////////////
const float Stats::MIN_HP_VALUE = 20.0f;
const float Stats::MAX_HP_VALUE = 1000.0f;
const float Stats::MIN_CAPACITY_VALUE = 30.0f;
const float Stats::MAX_CAPACITY_VALUE = 400.0f;
const float Stats::MIN_REGEN_RATE = 5.0f; //Points per second
const float Stats::MAX_REGEN_RATE = 25.0f;
const float Stats::MIN_DEFLECTION_VALUE = 0.0f;
const float Stats::MAX_DEFLECTION_VALUE = 30.0f;

//-----------------------------------------------------------------------------------
Stats::Stats(float defaultValue)
    : topSpeed(defaultValue)
    , acceleration(defaultValue)
    , handling(defaultValue)
    , braking(defaultValue)
    , damage(defaultValue)
    , shieldDisruption(defaultValue)
    , shotHoming(defaultValue)
    , rateOfFire(defaultValue)
    , hp(defaultValue)
    , shieldCapacity(defaultValue)
    , shieldRegen(defaultValue)
    , shotDeflection(defaultValue)
{

}

//-----------------------------------------------------------------------------------
//Counts only the positive boosts that could be dropped as items.
unsigned int Stats::GetTotalNumberOfDroppablePowerUps()
{
    unsigned int totalCount = 0;
    totalCount += static_cast<unsigned int>(topSpeed > 0 ? topSpeed : 0);
    totalCount += static_cast<unsigned int>(acceleration > 0 ? acceleration : 0);
    totalCount += static_cast<unsigned int>(handling > 0 ? handling : 0);
    totalCount += static_cast<unsigned int>(braking > 0 ? braking : 0);
    totalCount += static_cast<unsigned int>(damage > 0 ? damage : 0);
    totalCount += static_cast<unsigned int>(shieldDisruption > 0 ? shieldDisruption : 0);
    totalCount += static_cast<unsigned int>(shotHoming > 0 ? shotHoming : 0);
    totalCount += static_cast<unsigned int>(rateOfFire > 0 ? rateOfFire : 0);
    totalCount += static_cast<unsigned int>(hp > 0 ? hp : 0);
    totalCount += static_cast<unsigned int>(shieldCapacity > 0 ? shieldCapacity : 0);
    totalCount += static_cast<unsigned int>(shieldRegen > 0 ? shieldRegen : 0);
    totalCount += static_cast<unsigned int>(shotDeflection > 0 ? shotDeflection : 0);
    return totalCount;
}

//-----------------------------------------------------------------------------------
float* Stats::GetStatReference(PowerUpType type)
{
    switch (type)
    {
    case PowerUpType::TOP_SPEED:
        return &topSpeed;
    case PowerUpType::ACCELERATION:
        return &acceleration;
    case PowerUpType::HANDLING:
        return &handling;
    case PowerUpType::BRAKING:
        return &braking;
    case PowerUpType::DAMAGE:
        return &damage;
    case PowerUpType::SHIELD_DISRUPTION:
        return &shieldDisruption;
    case PowerUpType::SHOT_HOMING:
        return &shotHoming;
    case PowerUpType::RATE_OF_FIRE:
        return &rateOfFire;
    case PowerUpType::HP:
        return &hp;
    case PowerUpType::SHIELD_CAPACITY:
        return &shieldCapacity;
    case PowerUpType::SHIELD_REGEN:
        return &shieldRegen;
    case PowerUpType::SHOT_DEFLECTION:
        return &shotDeflection;
    default:
        ERROR_AND_DIE("Passed an invalid type to the stat reference function");
    }
}

//-----------------------------------------------------------------------------------
Stats& Stats::operator+=(const Stats& rhs)
{
    topSpeed += rhs.topSpeed;
    acceleration += rhs.acceleration;
    handling += rhs.handling;
    braking += rhs.braking;
    damage += rhs.damage;
    shieldDisruption += rhs.shieldDisruption;
    shotHoming += rhs.shotHoming;
    rateOfFire += rhs.rateOfFire;
    hp += rhs.hp;
    shieldCapacity += rhs.shieldCapacity;
    shieldRegen += rhs.shieldRegen;
    shotDeflection += rhs.shotDeflection;
    return *this;
}
