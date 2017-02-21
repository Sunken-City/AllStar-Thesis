#include "Game/Items/Chassis/TankChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
TankChassis::TankChassis()
{
    m_name = "Tank Chassis";
    m_statBonuses.topSpeed = -5.0f;
    m_statBonuses.handling = -1.0f;
    m_statBonuses.acceleration = -1.0f;
    m_statBonuses.hp = 10.0f;
    m_statBonuses.shieldCapacity = 5.0f;
}

//-----------------------------------------------------------------------------------
TankChassis::~TankChassis()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* TankChassis::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("TankChassisPickup");
}

//-----------------------------------------------------------------------------------
const SpriteResource* TankChassis::GetShipSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("TankChassis");
}

