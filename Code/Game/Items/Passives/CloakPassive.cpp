#include "Game/Items/Passives/CloakPassive.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Game/GameModes/GameMode.hpp"

//-----------------------------------------------------------------------------------
CloakPassive::CloakPassive()
{
    m_name = "Cloak";
}

//-----------------------------------------------------------------------------------
CloakPassive::~CloakPassive()
{
    if (m_owner)
    {
        Deactivate(NamedProperties::NONE);
    }
}

//-----------------------------------------------------------------------------------
void CloakPassive::Update(float deltaSeconds)
{
    static const float SPEED_THRESHOLD_FOR_CLOAK = 4.0f; //2.0f does a neat little flicker, 4.0f is more reasonable and smooth.
    static const float SPEED_THRESHOLD_FOR_CLOAK_SQUARED = SPEED_THRESHOLD_FOR_CLOAK * SPEED_THRESHOLD_FOR_CLOAK;
    float squaredMagnitude = m_owner->m_velocity.CalculateMagnitudeSquared();
    float alphaValue = Clamp<float>(squaredMagnitude / SPEED_THRESHOLD_FOR_CLOAK_SQUARED, 0.0f, 1.0f);
    m_owner->m_sprite->m_tintColor.SetAlphaFloat(alphaValue);
    m_owner->m_shieldSprite->m_tintColor.SetAlphaFloat(Min<float>(alphaValue, m_owner->m_shieldSprite->m_tintColor.GetAlphaFloat()));
    m_owner->m_shipTrail->m_colorOverride.SetAlphaFloat(alphaValue);

    if (!GameMode::GetCurrent()->m_isPlaying)
    {
        m_owner->m_sprite->m_tintColor.SetAlphaFloat(1.0f);
    }
}

//-----------------------------------------------------------------------------------
void CloakPassive::Activate(NamedProperties& parameters)
{
    ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_owner) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
}

//-----------------------------------------------------------------------------------
void CloakPassive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    m_owner->m_sprite->m_tintColor.SetAlphaFloat(1.0f);
    m_owner->m_shieldSprite->m_tintColor.SetAlphaFloat(1.0f);
    m_owner->m_shipTrail->m_colorOverride.SetAlphaFloat(1.0f);
}

//-----------------------------------------------------------------------------------
const SpriteResource* CloakPassive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("CloakPassive");
}

