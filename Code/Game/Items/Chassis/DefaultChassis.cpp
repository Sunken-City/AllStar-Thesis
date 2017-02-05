#include "Game/Items/Chassis/DefaultChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
DefaultChassis::DefaultChassis()
{
    m_name = "Standard Chassis";
}

//-----------------------------------------------------------------------------------
DefaultChassis::~DefaultChassis()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* DefaultChassis::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("DefaultChassisPickup");
}

//-----------------------------------------------------------------------------------
const SpriteResource* DefaultChassis::GetShipSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("DefaultChassis");
}

