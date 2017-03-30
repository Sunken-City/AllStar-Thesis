#include "Game/Items/Chassis/AttractorChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
AttractorChassis::AttractorChassis()
{
    m_name = "Attractor Chassis";
    m_statBonuses.shotHoming = 10.0f;
    m_statBonuses.rateOfFire = 3.0f;
    m_statBonuses.shieldDisruption = -3.0f;
    m_statBonuses.braking = -10.0f;
    m_statBonuses.shotDeflection = -5.0f;
}

//-----------------------------------------------------------------------------------
AttractorChassis::~AttractorChassis()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* AttractorChassis::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("AttractorChassisPickup");
}

//-----------------------------------------------------------------------------------
const SpriteResource* AttractorChassis::GetShipSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("AttractorChassis");
}

