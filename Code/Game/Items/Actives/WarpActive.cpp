#include "Game/Items/Actives/WarpActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/GameModes/GameMode.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/TheGame.hpp"
#include "Game/Pilots/Pilot.hpp"

const double WarpActive::SECONDS_UNTIL_WARP = 0.0;
const double WarpActive::MILISECONDS_UNTIL_WARP = SECONDS_UNTIL_WARP * 1000.0;

//-----------------------------------------------------------------------------------
WarpActive::WarpActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 0.25f;
    m_name = "Warp";
}

//-----------------------------------------------------------------------------------
WarpActive::~WarpActive()
{

}

//-----------------------------------------------------------------------------------
void WarpActive::Update(float deltaSeconds)
{
    if (m_isActive)
    {
        if (GetCurrentTimeMilliseconds() - m_lastActivatedMiliseconds > MILISECONDS_UNTIL_WARP)
        {
            ParticleSystem::PlayOneShotParticleEffect("Warped", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &m_transportee->m_transform);
            Pilot* pilot = m_transportee->m_pilot;
            Vector2 jumpedPosition = m_transportee->GetPosition() + pilot->m_inputMap.GetVector2("Right", "Up") * 5.0f;
            m_transportee->SetPosition(jumpedPosition);
            Deactivate(NamedProperties::NONE);
        }
    }
    else
    {
        Cooldown(deltaSeconds);
    }
}

//-----------------------------------------------------------------------------------
void WarpActive::Activate(NamedProperties& parameters)
{
    static SoundID warpingSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/swapDimensions.wav");
    if (CanActivate())
    {
        ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_transportee) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
        m_lastActivatedMiliseconds = GetCurrentTimeMilliseconds();
        m_isActive = true;
        m_energy = m_energy - m_costToActivate;

        ParticleSystem::PlayOneShotParticleEffect("Warping", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &m_transportee->m_transform);
        GameMode::GetCurrent()->PlaySoundAt(warpingSound, m_transportee->GetPosition());
    }
}

//-----------------------------------------------------------------------------------
void WarpActive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    m_isActive = false;
}

//-----------------------------------------------------------------------------------
const SpriteResource* WarpActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("TeleportActive");
}

