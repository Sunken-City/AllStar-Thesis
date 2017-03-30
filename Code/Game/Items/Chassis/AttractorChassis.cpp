#include "Game/Items/Chassis/AttractorChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/Entities/Ship.hpp"
#include "Game/Entities/PlayerShip.hpp"

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


//-----------------------------------------------------------------------------------
void AttractorChassis::Activate(NamedProperties& parameters)
{
    ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_owner) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
    PlayerShip* player = (PlayerShip*)m_owner;
    player->m_warpFreebieActive.m_costToActivate = 0.166666667f;
}

//-----------------------------------------------------------------------------------
void AttractorChassis::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    PlayerShip* player = (PlayerShip*)m_owner;
    player->m_warpFreebieActive.m_costToActivate = 0.25f;
}

