#include "Game/Entities/Projectiles/PlasmaBall.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "../TextSplash.hpp"

//-----------------------------------------------------------------------------------
PlasmaBall::PlasmaBall(Entity* owner, float degreesOffset /*= 0.0f*/, float damage /*= 1.0f*/, float disruption /*= 0.0f*/, float homing /*= 0.0f*/, MovementBehavior behavior /*= STRAIGHT*/)
    : Projectile(owner, degreesOffset, damage, disruption, homing)
    , m_behavior(behavior)
{
    m_sprite = new Sprite("PlasmaBall", TheGame::BULLET_LAYER_BLOOM);
    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetScale(Vector2(1.5f));
    m_sprite->m_tintColor = ((Ship*)owner)->m_factionColor;
    m_sprite->m_tintColor.SetAlphaFloat(0.3f);
    CalculateCollisionRadius();

    m_trail = new RibbonParticleSystem("BeamTrail", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(), &m_transform);
    m_trail->m_colorOverride = m_sprite->m_tintColor;
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
    m_lifeSpan = 1.0f;
}

//-----------------------------------------------------------------------------------
PlasmaBall::~PlasmaBall()
{
    delete m_trail;
}

//-----------------------------------------------------------------------------------
bool PlasmaBall::FlushParticleTrailIfExists()
{
    m_trail->Flush();
    return true;
}

//-----------------------------------------------------------------------------------
void PlasmaBall::Update(float deltaSeconds)
{
    static const float waveDegreesPerSecond = 1080.0f;
    static const float spriteAngularVelocity = 360.0f;
    static const float amplitude = 3.0f;
    const Vector2 muzzlePerpendicular = Vector2(-m_muzzleDirection.y, m_muzzleDirection.x);

    float newRotationDegrees = m_transform.GetWorldRotationDegrees() + (spriteAngularVelocity * deltaSeconds);
    m_transform.SetRotationDegrees(newRotationDegrees);

    switch (m_behavior)
    {
    case PlasmaBall::LEFT_WAVE:
        m_velocity += muzzlePerpendicular * (MathUtils::CosDegrees(m_age * waveDegreesPerSecond) * amplitude);
        //TextSplash::CreateTextSplash(Stringf("%.2f", m_age), m_transform, Vector2::UNIT_X);
        break;
    case PlasmaBall::RIGHT_WAVE:
        m_velocity += muzzlePerpendicular * (MathUtils::CosDegrees(m_age * waveDegreesPerSecond) * -amplitude);
        break;
    case PlasmaBall::STRAIGHT:
    case PlasmaBall::NUM_BEHAVIORS:
    default:
        break;
    }

    Projectile::Update(deltaSeconds);
}
