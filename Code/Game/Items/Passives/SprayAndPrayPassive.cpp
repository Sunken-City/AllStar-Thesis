#include "Game/Items/Passives/SprayAndPrayPassive.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
SprayAndPrayPassive::SprayAndPrayPassive()
{
    m_name = "Spray & Pray";
    m_statBonuses.damage = -5.0f;
    m_statBonuses.rateOfFire = 10.0f;
}

//-----------------------------------------------------------------------------------
void SprayAndPrayPassive::Activate(NamedProperties& parameters)
{
    ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_owner) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
}

//-----------------------------------------------------------------------------------
void SprayAndPrayPassive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
}

//-----------------------------------------------------------------------------------
const SpriteResource* SprayAndPrayPassive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("SprayAndPrayPassive");
}
