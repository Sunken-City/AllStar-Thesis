#include "Game/Items/PowerUp.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include <string>

//-----------------------------------------------------------------------------------
PowerUp::PowerUp()
    : Item()
{

}

//-----------------------------------------------------------------------------------
PowerUp::~PowerUp()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* PowerUp::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource(std::string(GetPowerUpSpriteResourceName()));
}

//-----------------------------------------------------------------------------------
const char* PowerUp::GetPowerUpSpriteResourceName()
{
    switch (m_type)
    {
    case TOP_SPEED:
        return "TopSpeed";
    case ACCELERATION:
        return "Acceleration";
    case AGILITY:
        return "Agility";
    case BRAKING:
        return "Braking";
    case DAMAGE:
        return "Damage";
    case SHIELD_DISRUPTION:
        return "ShieldDisruption";
    case SHIELD_PENETRATION:
        return "ShieldPenetration";
    case RATE_OF_FIRE:
        return "RateOfFire";
    case HP:
        return "HP";
    case SHIELD_CAPACITY:
        return "ShieldCapacity";
    case SHIELD_REGEN:
        return "ShieldRegen";
    case SHOT_DEFLECTION:
        return "ShotDeflection";
    default:
        ERROR_RECOVERABLE("Invalid Pickup type");
    }
}

