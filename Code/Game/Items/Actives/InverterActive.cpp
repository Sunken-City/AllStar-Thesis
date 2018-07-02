#include "Game/Items/Actives/InverterActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"

const double InverterActive::SECONDS_DURATION = 0.0f;//TheGame::SHIELD_ACTIVE_DURATION;
const double InverterActive::MILISECONDS_DURATION = SECONDS_DURATION * 1000.0f;

//-----------------------------------------------------------------------------------
InverterActive::InverterActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 0.1f;
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
        m_isActive = true;
        m_lastActivatedMiliseconds = GetCurrentTimeMilliseconds();
        m_energy -= m_costToActivate;

        Ship* ship = nullptr;
        ASSERT_OR_DIE(parameters.Get<Ship*>("ShipPtr", ship) == PGR_SUCCESS, "Wasn't able to grab the ship when activating a passive effect.");
        std::vector<Entity*> nearbyEntities = GameMode::GetCurrent()->GetEntitiesInRadiusSquared(ship->GetPosition(), 9.0f);
        for (Entity* entity : nearbyEntities)
        {
            if (entity->IsProjectile() && entity->m_owner != ship)
            {
                Projectile* projectile = (Projectile*)entity;
                projectile->m_owner = ship;
                projectile->m_velocity = -projectile->m_velocity;
                projectile->m_age = 0.0f;
            }
        }

        ParticleSystem::PlayOneShotParticleEffect("Forcefield", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &ship->m_transform);
    }
}

//-----------------------------------------------------------------------------------
void InverterActive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    m_isActive = false;
}

//-----------------------------------------------------------------------------------
const SpriteResource* InverterActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("InverterActive");
}
