#include "Game/Items/Passives/SharpshooterPassive.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
SharpshooterPassive::SharpshooterPassive()
{
    m_name = "Sharpshooter";
    m_statBonuses.damage = 5.0f;
    m_statBonuses.rateOfFire = -10.0f;
}

//-----------------------------------------------------------------------------------
void SharpshooterPassive::Activate(NamedProperties& parameters)
{
    ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_owner) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
}

//-----------------------------------------------------------------------------------
void SharpshooterPassive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
}

//-----------------------------------------------------------------------------------
const SpriteResource* SharpshooterPassive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("SharpshooterPassive");
}
