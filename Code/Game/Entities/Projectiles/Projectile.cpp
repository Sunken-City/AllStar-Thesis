#include "Game/Entities/Projectiles/Projectile.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"
#include <algorithm>
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "../PlayerShip.hpp"

//-----------------------------------------------------------------------------------
Projectile::Projectile(Entity* owner, float degreesOffset /*= 0.0f*/, float damage /*= 1.0f*/, float disruption /*= 0.0f*/, float homing /*= 0.0f*/) : Entity()
    , m_speed(10.0f)
    , m_damage(damage)
    , m_disruption(disruption)
    , m_shotHoming(homing)
    , m_lifeSpan(2.0f)
{
    m_owner = owner;
    m_collidesWithBullets = false;
    m_staysWithinBounds = false;
}

//-----------------------------------------------------------------------------------
Projectile::~Projectile()
{
}

//-----------------------------------------------------------------------------------
void Projectile::Update(float deltaSeconds)
{
    float mass = 1.0f;
    Entity::Update(deltaSeconds);
    if (m_age < m_lifeSpan)
    {
        Vector2 position = GetPosition();
        Vector2 accelerationDueToImpulses = m_sumOfImpulses / mass;
        m_velocity += (accelerationDueToImpulses * deltaSeconds);
        position += m_velocity * deltaSeconds;
        SetPosition(position);
        m_sumOfImpulses = Vector2::ZERO; //Only applied for a frame.
        
        m_sprite->m_transform.SetRotationDegrees(-m_velocity.CalculateThetaDegrees() + 90.0f);
    }
    else
    {
        m_isDead = true;
    }
}

//-----------------------------------------------------------------------------------
void Projectile::ResolveCollision(Entity* otherEntity)
{
    Entity::ResolveCollision(otherEntity);
    if (otherEntity != m_owner && otherEntity->m_collidesWithBullets && !otherEntity->m_isDead)
    {
        float damageDealt = otherEntity->TakeDamage(m_damage, m_disruption);
        if (m_reportDPSToPlayer)
        {
            PlayerShip* player = dynamic_cast<PlayerShip*>(m_owner);
            player->m_totalDamageDone += damageDealt;
        }
        this->m_isDead = true;
        ParticleSystem::PlayOneShotParticleEffect("Collision", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(GetPosition()), nullptr, otherEntity->GetCollisionSpriteResource());
    }
}

//-----------------------------------------------------------------------------------
void Projectile::ApplyImpulse(const Vector2& appliedAcceleration)
{
    const float mass = 1.0f;
    m_sumOfImpulses += mass * appliedAcceleration;
}

