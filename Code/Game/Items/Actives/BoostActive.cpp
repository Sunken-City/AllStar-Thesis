#include "Game/Items/Actives/BoostActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"
#include "Game/Entities/PlayerShip.hpp"

const double BoostActive::SECONDS_DURATION = 1.0f;
const double BoostActive::MILISECONDS_DURATION = SECONDS_DURATION * 1000.0f;

//-----------------------------------------------------------------------------------
BoostActive::BoostActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 0.20f;
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
        float diffInTime = GetCurrentTimeMilliseconds() - m_lastActivatedMiliseconds;
        float t = MathUtils::EaseInOut2(diffInTime / MILISECONDS_DURATION);
        m_owner->m_transform.SetScale(MathUtils::Lerp(t, PlayerShip::DEFAULT_SCALE, PlayerShip::DEFAULT_SCALE * Vector2(0.75f, 1.25f)));

        if (diffInTime > MILISECONDS_DURATION)
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
        m_statBonuses.topSpeed = 20.0f;
        m_statBonuses.acceleration = 10.0f;
        m_statBonuses.handling = -10.0f;
        m_isActive = true;
        m_lastActivatedMiliseconds = GetCurrentTimeMilliseconds();
        m_energy -= m_costToActivate;

        Ship* ship = nullptr;
        ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", ship) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
        ParticleSystem::PlayOneShotParticleEffect("Boost", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &ship->m_transform);
        m_owner = dynamic_cast<PlayerShip*>(ship);
        m_owner->m_collisionDamageAmount += BOOST_DAMAGE_PER_FRAME;
        m_owner->m_velocity += Vector2::DegreesToDirection(-m_owner->m_sprite->m_transform.GetWorldRotationDegrees(), Vector2::ZERO_DEGREES_UP) * 10.0f;
    }
}

//-----------------------------------------------------------------------------------
void BoostActive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    m_owner->m_transform.SetScale(PlayerShip::DEFAULT_SCALE);
    m_statBonuses.topSpeed = 0.0f;
    m_statBonuses.acceleration = 0.0f;
    m_statBonuses.handling = 0.0f;
    m_statBonuses.braking = 0.0f;
    m_isActive = false;
    m_owner->m_collisionDamageAmount -= BOOST_DAMAGE_PER_FRAME;
    if (fabs(m_owner->m_collisionDamageAmount) < 0.25f)
    {
        m_owner->m_collisionDamageAmount = 0.0f;
    }
}

//-----------------------------------------------------------------------------------
const SpriteResource* BoostActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("BoostActive");
}
