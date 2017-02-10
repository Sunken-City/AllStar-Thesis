#include "Game/Items/Chassis/BlackHoleChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
BlackHoleChassis::BlackHoleChassis()
{
    m_name = "Black Hole Chassis";
    m_statBonuses.shotHoming = 10.0f;
    m_statBonuses.damage = -2.0f;
    m_statBonuses.shotDeflection = -10.0f;
}

//-----------------------------------------------------------------------------------
BlackHoleChassis::~BlackHoleChassis()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* BlackHoleChassis::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("BlackHoleChassisPickup");
}

//-----------------------------------------------------------------------------------
const SpriteResource* BlackHoleChassis::GetShipSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("BlackHoleChassis");
}

