#include "Game/Entities/Projectiles/LeftHookLaser.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"

const float LeftHookLaser::KNOCKBACK_MAGNITUDE = 10.0f;

//-----------------------------------------------------------------------------------
LeftHookLaser::LeftHookLaser(Entity* owner, float degreesOffset /*= 0.0f*/, float damage /*= 1.0f*/, float disruption /*= 0.0f*/, float homing /*= 0.0f*/) 
    : Projectile(owner, degreesOffset, damage, disruption, homing)
{
    m_lifeSpan = 0.6f;
    m_sprite = new Sprite("LeftHookLaser", TheGame::BULLET_LAYER_BLOOM);
    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetScale(Vector2(1.5f));
    m_sprite->m_material = owner->m_sprite->m_material;
    m_sprite->m_tintColor.SetAlphaFloat(1.0f);
    CalculateCollisionRadius();
    SetPosition(owner->GetMuzzlePosition());
    m_spawnPosition = owner->GetMuzzlePosition();

    float parentRotationDegrees = m_owner->m_transform.GetWorldRotationDegrees();
    float totalRotationDegrees = parentRotationDegrees + degreesOffset;
    m_transform.SetRotationDegrees(totalRotationDegrees);
    m_muzzleDirection = Vector2::DegreesToDirection(-totalRotationDegrees, Vector2::ZERO_DEGREES_UP);

    float ownerForwardSpeed = Vector2::Dot(m_muzzleDirection, m_owner->m_velocity);
    ownerForwardSpeed = std::max<float>(0.0f, ownerForwardSpeed);
    float adjustedSpeed = m_speed + ownerForwardSpeed;

    Vector2 muzzleVelocity = m_muzzleDirection * adjustedSpeed;
    m_velocity = muzzleVelocity;

    Vector2 velocityDir = muzzleVelocity.GetNorm();
    Vector2 leftDir = Vector2(-velocityDir.y, velocityDir.x);
    m_acceleration = leftDir * 400.0f;

    if (MathUtils::CoinFlip())
    {
        m_acceleration *= -1.0f;
    }
}

//-----------------------------------------------------------------------------------
LeftHookLaser::~LeftHookLaser()
{

}

//-----------------------------------------------------------------------------------
void LeftHookLaser::Update(float deltaSeconds)
{
    static const float SPRITE_ANGULAR_VELOCITY = 1080.0f;
    static const float AMPLITUDE = 2.0f;
    static const float AGE_OFFSET = 1.0f;

    float newRotationDegrees = m_transform.GetWorldRotationDegrees() + (SPRITE_ANGULAR_VELOCITY * deltaSeconds);
    m_transform.SetRotationDegrees(newRotationDegrees);

    Entity::Update(deltaSeconds);
    if (m_age < m_lifeSpan)
    {
        if (m_age > (m_lifeSpan * 0.5f))
        {
            m_velocity += m_acceleration * deltaSeconds;
        }
        m_lastLifespan = m_age;

        Vector2 position = GetPosition();
        position += m_velocity * deltaSeconds;
        SetPosition(position);
    }
    else
    {
        m_isDead = true;
    }
}

//-----------------------------------------------------------------------------------
float LeftHookLaser::GetKnockbackMagnitude()
{
    return KNOCKBACK_MAGNITUDE;
}
