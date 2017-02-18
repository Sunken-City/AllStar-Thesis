#include "Game/Entities/Projectiles/Laser.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"

//-----------------------------------------------------------------------------------
Laser::Laser(Entity* owner, float degreesOffset /*= 0.0f*/, float damage /*= 1.0f*/, float disruption /*= 0.0f*/, float homing /*= 0.0f*/)
    : Projectile(owner, degreesOffset, damage, disruption, homing)
{
    m_sprite = new Sprite("Laser", TheGame::BULLET_LAYER_BLOOM);
    m_sprite->m_transform.SetScale(Vector2(1.5f));
    //m_sprite->m_tintColor = ((Ship*)owner)->m_factionColor;
    m_sprite->m_material = owner->m_sprite->m_material;
    m_sprite->m_tintColor.SetAlphaFloat(1.0f);
    CalculateCollisionRadius();
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
Laser::~Laser()
{

}
