#include "Game/Items/Chassis/TankChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
TankChassis::TankChassis()
{
    m_name = "Tank Chassis";
    m_statBonuses.topSpeed = 10.0f;
    m_statBonuses.handling = -1.0f;
    m_statBonuses.damage = -2.0f;
    m_statBonuses.shotHoming = -3.0f;
    m_statBonuses.rateOfFire = -1.0f;
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

