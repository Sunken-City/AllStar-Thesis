#include "Game/Items/Actives/TeleportActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/GameModes/GameMode.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/TheGame.hpp"

const double TeleportActive::SECONDS_UNTIL_WARP = 0.0;
const double TeleportActive::MILISECONDS_UNTIL_WARP = SECONDS_UNTIL_WARP * 1000.0f;

//-----------------------------------------------------------------------------------
TeleportActive::TeleportActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 0.5f;
    m_name = "Teleport";
}

//-----------------------------------------------------------------------------------
TeleportActive::~TeleportActive()
{

}

//-----------------------------------------------------------------------------------
void TeleportActive::Update(float deltaSeconds)
{
    if (m_isActive)
    {
        if (GetCurrentTimeMilliseconds() - m_lastActivatedMiliseconds > MILISECONDS_UNTIL_WARP)
        {
            ParticleSystem::PlayOneShotParticleEffect("Warped", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &m_transportee->m_transform);
            m_transportee->SetPosition(GameMode::GetCurrent()->GetRandomLocationInArena());
            Deactivate(NamedProperties::NONE);
        }
    }
    else
    {
        Cooldown(deltaSeconds);
    }
}

//-----------------------------------------------------------------------------------
void TeleportActive::Activate(NamedProperties& parameters)
{
    static SoundID warpingSound = AudioSystem::instance->CreateOrGetSound("Data/SFX/teleport.ogg");
    if (CanActivate())
    {
        ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_transportee) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
        m_lastActivatedMiliseconds = GetCurrentTimeMilliseconds();
        m_isActive = true;
        m_energy = m_energy - m_costToActivate;

        ParticleSystem::PlayOneShotParticleEffect("Warping", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &m_transportee->m_transform);
        GameMode::GetCurrent()->PlaySoundAt(warpingSound, m_transportee->GetPosition(), TheGame::TELEPORT_VOLUME, MathUtils::GetRandomFloat(0.9f, 1.1f));
    }
}

//-----------------------------------------------------------------------------------
void TeleportActive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    m_isActive = false;
}

//-----------------------------------------------------------------------------------
const SpriteResource* TeleportActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("WarpActive");
}

