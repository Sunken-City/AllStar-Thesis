#include "Game/Entities/Projectiles/Missile.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"

//-----------------------------------------------------------------------------------
Missile::Missile(Entity* owner, float degreesOffset, float power, float disruption, float homing)
    : Projectile(owner, degreesOffset, power, disruption, homing)
{
    m_speed = 4.0f;

    m_sprite = new Sprite("Missile", TheGame::PLAYER_BULLET_LAYER);
    m_sprite->m_tintColor = owner->m_sprite->m_tintColor;
    CalculateCollisionRadius();
    m_missileTrail = new RibbonParticleSystem("MissileTrail", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(), &m_sprite->m_transform);
    m_missileTrail->m_colorOverride = RGBA::GRAY;
    SetPosition(owner->GetMuzzlePosition());

    float parentRotationDegrees = m_owner->m_sprite->m_transform.GetWorldRotationDegrees();
    float totalRotationDegrees = parentRotationDegrees + degreesOffset;
    m_sprite->m_transform.SetRotationDegrees(totalRotationDegrees);
    Vector2 direction = Vector2::DegreesToDirection(-totalRotationDegrees, Vector2::ZERO_DEGREES_UP);

    float ownerForwardSpeed = Vector2::Dot(direction, m_owner->m_velocity);
    ownerForwardSpeed = std::max<float>(0.0f, ownerForwardSpeed);
    float adjustedSpeed = m_speed + ownerForwardSpeed;

    Vector2 muzzleVelocity = direction * adjustedSpeed;
    m_velocity = muzzleVelocity;
}

//-----------------------------------------------------------------------------------
Missile::~Missile()
{
    delete m_missileTrail;
}
