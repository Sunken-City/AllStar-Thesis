#include "Game/Items/Actives/WarpActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/GameModes/GameMode.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/TheGame.hpp"

const double WarpActive::SECONDS_UNTIL_WARP = 1.0;
const double WarpActive::MILISECONDS_UNTIL_WARP = SECONDS_UNTIL_WARP * 1000.0f;

//-----------------------------------------------------------------------------------
WarpActive::WarpActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 1.0f;
}

//-----------------------------------------------------------------------------------
WarpActive::~WarpActive()
{

}

//-----------------------------------------------------------------------------------
void WarpActive::Update(float deltaSeconds)
{
    if (m_isWarping)
    {
        if (GetCurrentTimeMilliseconds() - m_warpTimeStart > MILISECONDS_UNTIL_WARP)
        {
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
void WarpActive::Activate(NamedProperties& parameters)
{
    if (m_energy >= m_costToActivate && !m_isWarping)
    {
        ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", m_transportee) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
        m_warpTimeStart = GetCurrentTimeMilliseconds();
        m_isWarping = true;
        ParticleSystem::PlayOneShotParticleEffect("Death", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &m_transportee->m_transform);
    }
}

//-----------------------------------------------------------------------------------
void WarpActive::Deactivate(NamedProperties& parameters)
{
    m_isWarping = false;
    m_energy = m_energy - m_costToActivate;
}

//-----------------------------------------------------------------------------------
const SpriteResource* WarpActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("WarpActive");
}

