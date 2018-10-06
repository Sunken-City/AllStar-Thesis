#include "Game/Items/Actives/ReflectorActive.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"
#include "Game/Entities/Projectiles/Projectile.hpp"

const double ReflectorActive::SECONDS_DURATION = TheGame::REFLECTOR_ACTIVE_DURATION;
const double ReflectorActive::MILISECONDS_DURATION = SECONDS_DURATION * 1000.0f;

//-----------------------------------------------------------------------------------
ReflectorActive::ReflectorActive()
{
    m_energyRestorationPerSecond = 0.05f;
    m_costToActivate = 0.20f;
    m_name = "Reflector";
}

//-----------------------------------------------------------------------------------
ReflectorActive::~ReflectorActive()
{

}

//-----------------------------------------------------------------------------------
void ReflectorActive::Update(float deltaSeconds)
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
void ReflectorActive::Activate(NamedProperties& parameters)
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
                projectile->m_damage *= 1.25f;
                projectile->m_age = 0.0f;
            }
        }

        ParticleSystem::PlayOneShotParticleEffect("Reflector", TheGame::BACKGROUND_PARTICLES_BLOOM_LAYER, Transform2D(), &ship->m_transform);
    }
}

//-----------------------------------------------------------------------------------
void ReflectorActive::Deactivate(NamedProperties& parameters)
{
    UNUSED(parameters);
    m_isActive = false;
}

//-----------------------------------------------------------------------------------
const SpriteResource* ReflectorActive::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("ReflectorActive");
}
