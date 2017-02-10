#include "Game/Items/Actives/BoostActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"

const double BoostActive::SECONDS_DURATION = 1.0f;
const double BoostActive::MILISECONDS_DURATION = SECONDS_DURATION * 1000.0f;

//-----------------------------------------------------------------------------------
BoostActive::BoostActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 0.0f;
    m_name = "Boost";
}

//-----------------------------------------------------------------------------------
BoostActive::~BoostActive()
{

}

//-----------------------------------------------------------------------------------
void BoostActive::Update(float deltaSeconds)
{
    if (m_isActive)
    {
        if (GetCurrentTimeMilliseconds() - m_lastActivatedMiliseconds > MILISECONDS_DURATION)
        {
            Deactivate(NamedProperties::NONE);
        }
    }
    else
    {
        Cooldown(deltaSeconds);
    }
}

//-----------------------------------------------------------------------------------
void BoostActive::Activate(NamedProperties& parameters)
{
    if (CanActivate())
    {
        m_statBonuses.topSpeed = 10.0f;
        m_statBonuses.acceleration = 10.0f;
        m_statBonuses.handling = -10.0f;
        //m_statBonuses.braking = -10.0f;
        m_isActive = true;
        m_lastActivatedMiliseconds = GetCurrentTimeMilliseconds();
        m_energy -= m_costToActivate;

        Ship* ship = nullptr;
        ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", ship) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
        ParticleSystem::PlayOneShotParticleEffect("Buff", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &ship->m_transform);
    }
}

//-----------------------------------------------------------------------------------
void BoostActive::Deactivate(NamedProperties& parameters)
{
    m_statBonuses.topSpeed = 0.0f;
    m_statBonuses.acceleration = 0.0f;
    m_statBonuses.handling = 0.0f;
    m_statBonuses.braking = 0.0f;
    m_isActive = false;
}

//-----------------------------------------------------------------------------------
const SpriteResource* BoostActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("BoostActive");
}
