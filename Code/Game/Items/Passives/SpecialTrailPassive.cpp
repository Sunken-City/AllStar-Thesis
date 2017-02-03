#include "Game/Items/Passives/SpecialTrailPassive.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
SpecialTrailPassive::SpecialTrailPassive()
{
    m_name = "Special Trail";
    m_trailID = MathUtils::GetRandomIntFromZeroTo(MAX_TRAIL_IDS);
    m_statBonuses.topSpeed = 2;
    m_statBonuses.acceleration = 2;
}

//-----------------------------------------------------------------------------------
void SpecialTrailPassive::Activate(NamedProperties& parameters)
{
    ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_owner) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
    m_owner->m_shipTrail->m_emitters[0]->m_spriteOverride = ResourceDatabase::instance->GetSpriteResource("SpecialTrailLol");
}

//-----------------------------------------------------------------------------------
void SpecialTrailPassive::Deactivate(NamedProperties& parameters)
{
    m_owner->m_shipTrail->m_emitters[0]->m_spriteOverride = ResourceDatabase::instance->GetSpriteResource("BeamTrail");
}

//-----------------------------------------------------------------------------------
const SpriteResource* SpecialTrailPassive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource(Stringf("SpecialTrailPassive%s", "Lol"));
}
