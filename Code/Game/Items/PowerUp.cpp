#include "Game/Items/PowerUp.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include <string>

//-----------------------------------------------------------------------------------
PowerUp::PowerUp(PowerUpType type)
    : Item()
    , m_powerUpType(type)
{
    SetStatChangeFromType(m_powerUpType);
}

//-----------------------------------------------------------------------------------
PowerUp::PowerUp(Player::Stats statChanges)
    : Item()
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
    case PowerUpType::ACCELERATION:
        m_statChanges.acceleration = 1;
    case PowerUpType::AGILITY:
        m_statChanges.agility = 1;
    case PowerUpType::BRAKING:
        m_statChanges.braking = 1;
    case PowerUpType::DAMAGE:
        m_statChanges.damage = 1;
    case PowerUpType::SHIELD_DISRUPTION:
        m_statChanges.shieldDisruption = 1;
    case PowerUpType::SHIELD_PENETRATION:
        m_statChanges.shieldPenetration = 1;
    case PowerUpType::RATE_OF_FIRE:
        m_statChanges.rateOfFire = 1;
    case PowerUpType::HP:
        m_statChanges.hp = 1;
    case PowerUpType::SHIELD_CAPACITY:
        m_statChanges.shieldCapacity = 1;
    case PowerUpType::SHIELD_REGEN:
        m_statChanges.shieldRegen = 1;
    case PowerUpType::SHOT_DEFLECTION:
        m_statChanges.shotDeflection = 1;
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
    case PowerUpType::BRAKING:
        return "Braking";
    case PowerUpType::DAMAGE:
        return "Damage";
    case PowerUpType::SHIELD_DISRUPTION:
        return "ShieldDisruption";
    case PowerUpType::SHIELD_PENETRATION:
        return "ShieldPenetration";
    case PowerUpType::RATE_OF_FIRE:
        return "RateOfFire";
    case PowerUpType::HP:
        return "HP";
    case PowerUpType::SHIELD_CAPACITY:
        return "ShieldCapacity";
    case PowerUpType::SHIELD_REGEN:
        return "ShieldRegen";
    case PowerUpType::SHOT_DEFLECTION:
        return "ShotDeflection";
    case PowerUpType::HYBRID:
        return "Hybrid";
    default:
        ERROR_RECOVERABLE("Invalid Pickup type");
    }
}

//-----------------------------------------------------------------------------------
void PowerUp::ApplyPickupEffect(Player* player)
{
    player->m_stats += m_statChanges;
}

