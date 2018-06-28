#include "Game/Items/Actives/InverterActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"

const double InverterActive::SECONDS_DURATION = TheGame::SHIELD_ACTIVE_DURATION;
const double InverterActive::MILISECONDS_DURATION = SECONDS_DURATION * 1000.0f;

//-----------------------------------------------------------------------------------
InverterActive::InverterActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 1.0f;
    m_name = "Inverter";
}

//-----------------------------------------------------------------------------------
InverterActive::~InverterActive()
{

}

//-----------------------------------------------------------------------------------
void InverterActive::Update(float deltaSeconds)
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
void InverterActive::Activate(NamedProperties& parameters)
{
    if (CanActivate())
    {
        m_statBonuses.shotDeflection = 30.0f;
        m_statBonuses.shieldRegen = 30.0f;
        m_statBonuses.shieldCapacity = 10.0f;
        m_isActive = true;
        m_lastActivatedMiliseconds = GetCurrentTimeMilliseconds();
        m_energy -= m_costToActivate;

        Ship* ship = nullptr;
        ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", ship) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
        ParticleSystem::PlayOneShotParticleEffect("Forcefield", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &ship->m_transform);
    }
}

//-----------------------------------------------------------------------------------
void InverterActive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    m_statBonuses.shotDeflection = 0.0f;
    m_statBonuses.shieldRegen = 0.0f;
    m_statBonuses.shieldCapacity = 0.0f;
    m_isActive = false;
}

//-----------------------------------------------------------------------------------
const SpriteResource* InverterActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("ShieldActive");
}
