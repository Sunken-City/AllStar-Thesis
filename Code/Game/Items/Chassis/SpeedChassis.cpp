#include "Game/Items/Chassis/SpeedChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
SpeedChassis::SpeedChassis()
{
    m_name = "Speed Chassis";
    m_statBonuses.topSpeed = 10.0f;
    m_statBonuses.handling = -0.5f;
}

//-----------------------------------------------------------------------------------
SpeedChassis::~SpeedChassis()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* SpeedChassis::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("SpeedChassisPickup");
}

//-----------------------------------------------------------------------------------
const SpriteResource* SpeedChassis::GetShipSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("SpeedChassis");
}

