#include "Game/Items/Chassis/PowerChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
PowerChassis::PowerChassis()
{
    m_name = "Power Chassis";
    m_statBonuses.damage = 5.0f;
    m_statBonuses.shieldCapacity = -5.0f;
}

//-----------------------------------------------------------------------------------
PowerChassis::~PowerChassis()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* PowerChassis::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("PowerChassisPickup");
}

//-----------------------------------------------------------------------------------
const SpriteResource* PowerChassis::GetShipSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("PowerChassis");
}

