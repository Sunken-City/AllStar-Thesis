#include "Game/Items/Chassis/GlassCannonChassis.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
GlassCannonChassis::GlassCannonChassis()
{
    m_name = "Glass Cannon Chassis";
    m_statBonuses.topSpeed = 5.0f;
    m_statBonuses.damage = 7.0f;
    m_statBonuses.shotHoming = 3.0f;
    m_statBonuses.shotDeflection = -3.0f;
    m_statBonuses.handling = -1.0f;
    m_statBonuses.shieldCapacity = -5.0f;
    m_statBonuses.hp = -3.0f;
    m_statBonuses.rateOfFire = 2.0f;
}

//-----------------------------------------------------------------------------------
GlassCannonChassis::~GlassCannonChassis()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* GlassCannonChassis::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("GlassCannonChassisPickup");
}

//-----------------------------------------------------------------------------------
const SpriteResource* GlassCannonChassis::GetShipSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("GlassCannonChassis");
}

