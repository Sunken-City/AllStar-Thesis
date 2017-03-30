#include "Game/Items/Chassis/TankChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Game/Entities/Ship.hpp"

//-----------------------------------------------------------------------------------
TankChassis::TankChassis()
{
    m_name = "Tank Chassis";
    m_statBonuses.topSpeed = -5.0f;
    m_statBonuses.handling = -2.0f;
    m_statBonuses.acceleration = -2.0f;
    m_statBonuses.damage = -2.0f;
    m_statBonuses.rateOfFire = -2.0f;
    m_statBonuses.hp = 5.0f;
    m_statBonuses.shieldCapacity = 10.0f;
    m_statBonuses.shotDeflection = 10.0f;
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

//-----------------------------------------------------------------------------------
void TankChassis::Activate(NamedProperties& parameters)
{
    ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_owner) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
    m_owner->m_collisionDamageAmount += COLLISION_DAMAGE_PER_COLLISION;
}

//-----------------------------------------------------------------------------------
void TankChassis::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    m_owner->m_collisionDamageAmount -= COLLISION_DAMAGE_PER_COLLISION;
    if (fabs(m_owner->m_collisionDamageAmount) < 0.25f)
    {
        m_owner->m_collisionDamageAmount = 0.0f;
    }
}

