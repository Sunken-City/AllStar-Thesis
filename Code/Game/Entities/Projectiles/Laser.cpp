#include "Game/Entities/Projectiles/Laser.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"

//-----------------------------------------------------------------------------------
Laser::Laser(Entity* owner, float degreesOffset /*= 0.0f*/, float damage /*= 1.0f*/, float disruption /*= 0.0f*/, float homing /*= 0.0f*/, MovementBehavior behavior /*= STRAIGHT*/) 
    : Projectile(owner, degreesOffset, damage, disruption, homing)
    , m_behavior(behavior)
{
    m_sprite = new Sprite("Laser", TheGame::BULLET_LAYER_BLOOM);
    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetScale(Vector2(1.5f));
    m_sprite->m_tintColor = ((Ship*)owner)->m_factionColor;
    //m_sprite->m_material = owner->m_sprite->m_material;
    m_sprite->m_tintColor.SetAlphaFloat(1.0f);
    CalculateCollisionRadius();
    SetPosition(owner->GetMuzzlePosition());

    float parentRotationDegrees = m_owner->m_transform.GetWorldRotationDegrees();
    float totalRotationDegrees = parentRotationDegrees + degreesOffset;
    m_transform.SetRotationDegrees(totalRotationDegrees);
    m_muzzleDirection = Vector2::DegreesToDirection(-totalRotationDegrees, Vector2::ZERO_DEGREES_UP);

    float ownerForwardSpeed = Vector2::Dot(m_muzzleDirection, m_owner->m_velocity);
    ownerForwardSpeed = std::max<float>(0.0f, ownerForwardSpeed);
    float adjustedSpeed = m_speed + ownerForwardSpeed;

    Vector2 muzzleVelocity = m_muzzleDirection * adjustedSpeed;
    m_velocity = muzzleVelocity;
}

//-----------------------------------------------------------------------------------
Laser::~Laser()
{

}

//-----------------------------------------------------------------------------------
void Laser::Update(float deltaSeconds)
{
    const Vector2 muzzlePerpendicular = Vector2(m_muzzleDirection.y, m_muzzleDirection.x);
    switch (m_behavior)
    {
    case Laser::LEFT_WAVE:
        m_velocity += muzzlePerpendicular * MathUtils::CosDegrees(m_age * 1080.0f);
        break;
    case Laser::RIGHT_WAVE:
        m_velocity += muzzlePerpendicular * -MathUtils::CosDegrees(m_age * 1080.0f);
        break;
    case Laser::STRAIGHT:
    case Laser::NUM_BEHAVIORS:
    default:
        break;
    }

    Projectile::Update(deltaSeconds);
}
