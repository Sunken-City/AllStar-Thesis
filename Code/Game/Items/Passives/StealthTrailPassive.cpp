#include "Game/Items/Passives/StealthTrailPassive.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"

//-----------------------------------------------------------------------------------
void StealthTrailPassive::Activate(NamedProperties& parameters)
{
    ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_owner) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
    m_owner->m_shipTrail->m_colorOverride.SetAlphaFloat(0.0f);
}

//-----------------------------------------------------------------------------------
void StealthTrailPassive::Deactivate(NamedProperties& parameters)
{
    m_owner->m_shipTrail->m_colorOverride.SetAlphaFloat(1.0f);
}
