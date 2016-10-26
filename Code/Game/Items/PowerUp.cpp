#include "Game/Items/PowerUp.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include <string>

//-----------------------------------------------------------------------------------
PowerUp::PowerUp(PowerUpType type)
    : Item(ItemType::POWER_UP)
    , m_powerUpType(type)
{
    if (m_powerUpType == PowerUpType::RANDOM)
    {
        m_powerUpType = static_cast<PowerUpType>(MathUtils::GetRandomIntFromZeroTo((int)PowerUpType::HYBRID));
    }
    SetStatChangeFromType(m_powerUpType);
}

//-----------------------------------------------------------------------------------
PowerUp::PowerUp(Stats statChanges)
    : Item(ItemType::POWER_UP)
    , m_powerUpType(PowerUpType::HYBRID)
    , m_statChanges(statChanges)
{

}

//-----------------------------------------------------------------------------------
PowerUp::~PowerUp()
{

}

//-----------------------------------------------------------------------------------
void PowerUp::SetStatChangeFromType(PowerUpType type)
{
    switch (type)
    {
    case PowerUpType::TOP_SPEED:
        m_statChanges.topSpeed = 1;
        break;
    case PowerUpType::ACCELERATION:
        m_statChanges.acceleration = 1;
        break;
    case PowerUpType::AGILITY:
        m_statChanges.agility = 1;
        break;
    case PowerUpType::WEIGHT:
        m_statChanges.weight = 1;
        break;
    case PowerUpType::DAMAGE:
        m_statChanges.damage = 1;
        break;
    case PowerUpType::SHIELD_DISRUPTION:
        m_statChanges.shieldDisruption = 1;
        break;
    case PowerUpType::SHIELD_PENETRATION:
        m_statChanges.shieldPenetration = 1;
        break;
    case PowerUpType::RATE_OF_FIRE:
        m_statChanges.rateOfFire = 1;
        break;
    case PowerUpType::HP:
        m_statChanges.hp = 1;
        break;
    case PowerUpType::SHIELD_CAPACITY:
        m_statChanges.shieldCapacity = 1;
        break;
    case PowerUpType::SHIELD_REGEN:
        m_statChanges.shieldRegen = 1;
        break;
    case PowerUpType::SHOT_DEFLECTION:
        m_statChanges.shotDeflection = 1;
        break;
    default:
        ERROR_RECOVERABLE("Invalid PowerUpType used for setting stats");
    }
}

//-----------------------------------------------------------------------------------
const SpriteResource* PowerUp::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource(std::string(GetPowerUpSpriteResourceName()));
}

//-----------------------------------------------------------------------------------
const char* PowerUp::GetPowerUpSpriteResourceName()
{
    switch (m_powerUpType)
    {
    case PowerUpType::TOP_SPEED:
        return "TopSpeed";
    case PowerUpType::ACCELERATION:
        return "Acceleration";
    case PowerUpType::AGILITY:
        return "Agility";
    case PowerUpType::WEIGHT:
        return "Weight";
    case PowerUpType::DAMAGE:
        return "Damage";
    case PowerUpType::SHIELD_DISRUPTION:
        return "ShieldDisruption";
    case PowerUpType::SHIELD_PENETRATION:
        return "ShieldPenetration";
    case PowerUpType::RATE_OF_FIRE:
        return "RateOfFire";
    case PowerUpType::HP:
        return "Hp";
    case PowerUpType::SHIELD_CAPACITY:
        return "ShieldCapacity";
    case PowerUpType::SHIELD_REGEN:
        return "ShieldRegen";
    case PowerUpType::SHOT_DEFLECTION:
        return "ShotDeflection";
    case PowerUpType::HYBRID:
        return "Hybrid";
    default:
        ERROR_AND_DIE("Invalid Pickup type");
    }
}

//-----------------------------------------------------------------------------------
void PowerUp::ApplyPickupEffect(PlayerShip* player)
{
    player->m_powerupStatModifiers += m_statChanges;
}

