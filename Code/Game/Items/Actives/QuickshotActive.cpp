#include "Game/Items/Actives/QuickshotActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Time/Time.hpp"

const double QuickshotActive::SECONDS_DURATION = 5.0f;
const double QuickshotActive::MILISECONDS_DURATION = SECONDS_DURATION * 1000.0f;

//-----------------------------------------------------------------------------------
QuickshotActive::QuickshotActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 1.0f;
}

//-----------------------------------------------------------------------------------
QuickshotActive::~QuickshotActive()
{

}

//-----------------------------------------------------------------------------------
void QuickshotActive::Update(float deltaSeconds)
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
void QuickshotActive::Activate(NamedProperties& parameters)
{
    if (CanActivate())
    {
        m_statBonuses.rateOfFire = 10.0f;
        m_isActive = true;
        m_lastActivatedMiliseconds = GetCurrentTimeMilliseconds();
        m_energy -= m_costToActivate;
    }
}

//-----------------------------------------------------------------------------------
void QuickshotActive::Deactivate(NamedProperties& parameters)
{
    m_statBonuses.rateOfFire = 0.0f;
    m_isActive = false;
}

//-----------------------------------------------------------------------------------
const SpriteResource* QuickshotActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("QuickshotActive");
}
