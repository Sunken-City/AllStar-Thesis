#include "Game/Entities/Projectiles/Missile.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Game/Entities/Ship.hpp"
#include "../PlayerShip.hpp"

const float Missile::KNOCKBACK_MAGNITUDE = 10.0f;

//-----------------------------------------------------------------------------------
Missile::Missile(Entity* owner, float degreesOffset, float damage, float disruption, float homing)
    : Projectile(owner, degreesOffset, damage, disruption, homing)
{
    m_speed = 5.5f;

    m_sprite = new AnimatedSprite("Missile", "Missile1", TheGame::BULLET_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetScale(Vector2(3.0f));
    m_sprite->m_tintColor = ((Ship*)owner)->m_factionColor;
    m_sprite->m_tintColor.SetAlphaFloat(1.0f);

    CalculateCollisionRadius();
    m_missileTrail = new RibbonParticleSystem("MissileTrail", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(), &m_transform);
    m_missileTrail->m_colorOverride = RGBA::GRAY;
    PlayerShip* player = dynamic_cast<PlayerShip*>(m_owner);
    if (player)
    {
        m_sprite->m_tintColor = RGBA::WHITE;
        m_missileTrail->m_colorOverride = RGBA::WHITE;
        m_missileTrail->m_emitters[0]->m_materialOverride = player->m_shipTrail->m_emitters[0]->m_materialOverride;
        m_missileTrail->m_emitters[0]->m_spriteOverride = player->m_shipTrail->m_emitters[0]->m_spriteOverride;
        m_sprite->m_material = player->m_sprite->m_material;
    }
    SetPosition(owner->GetMuzzlePosition());

    float parentRotationDegrees = m_owner->m_transform.GetWorldRotationDegrees();
    float totalRotationDegrees = parentRotationDegrees + degreesOffset;
    m_transform.SetRotationDegrees(totalRotationDegrees);
    Vector2 direction = Vector2::DegreesToDirection(-totalRotationDegrees, Vector2::ZERO_DEGREES_UP);

    float ownerForwardSpeed = Vector2::Dot(direction, m_owner->m_velocity);
    ownerForwardSpeed = std::max<float>(0.0f, ownerForwardSpeed);
    float adjustedSpeed = m_speed + ownerForwardSpeed;

    Vector2 muzzleVelocity = direction * adjustedSpeed;
    m_velocity = muzzleVelocity;

    m_lifeSpan = 1.0f;
}

//-----------------------------------------------------------------------------------
Missile::~Missile()
{
    delete m_missileTrail;
}

//-----------------------------------------------------------------------------------
bool Missile::FlushParticleTrailIfExists()
{
    m_missileTrail->Flush();
    return true;
}

//-----------------------------------------------------------------------------------
float Missile::GetKnockbackMagnitude()
{
    return KNOCKBACK_MAGNITUDE;
}

//-----------------------------------------------------------------------------------
void Missile::LockOn()
{
    if (!m_hasLockedOn)
    {
        m_lifeSpan += 1.0f;
    }
    m_hasLockedOn = true;
}
