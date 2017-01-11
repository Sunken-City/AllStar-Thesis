#include "Game/Stats.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const float Stats::CAPACITY_VALUE_PER_POINT = 5.0f;
const float Stats::HP_VALUE_PER_POINT = 5.0f;

//-----------------------------------------------------------------------------------
Stats::Stats(float defaultValue)
    : topSpeed(defaultValue)
    , acceleration(defaultValue)
    , handling(defaultValue)
    , braking(defaultValue)
    , damage(defaultValue)
    , shieldDisruption(defaultValue)
    , shieldPenetration(defaultValue)
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
    totalCount += static_cast<unsigned int>(shieldPenetration > 0 ? shieldPenetration : 0);
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
    case PowerUpType::SHIELD_PENETRATION:
        return &shieldPenetration;
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
    shieldPenetration += rhs.shieldPenetration;
    rateOfFire += rhs.rateOfFire;
    hp += rhs.hp;
    shieldCapacity += rhs.shieldCapacity;
    shieldRegen += rhs.shieldRegen;
    shotDeflection += rhs.shotDeflection;
    return *this;
}
