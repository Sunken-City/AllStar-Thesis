#include "Game/Entities/Projectiles/PlasmaBall.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entities/Ship.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "../TextSplash.hpp"

const float PlasmaBall::KNOCKBACK_MAGNITUDE = 8.0f;

//-----------------------------------------------------------------------------------
PlasmaBall::PlasmaBall(Entity* owner, float degreesOffset /*= 0.0f*/, float damage /*= 1.0f*/, float disruption /*= 0.0f*/, float homing /*= 0.0f*/, MovementBehavior behavior /*= STRAIGHT*/)
    : Projectile(owner, degreesOffset, damage, disruption, homing)
    , m_behavior(behavior)
{
    m_sprite = new Sprite("PlasmaBall", TheGame::BULLET_LAYER_BLOOM);
    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetScale(Vector2(1.5f));

    switch (m_behavior)
    {
    case PlasmaBall::LEFT_WAVE:
        m_wavePhase = 0.0f;
        m_sprite->m_tintColor = RGBA::BLUE;
        break;
    case PlasmaBall::RIGHT_WAVE:
        m_wavePhase = 180.0f;
        m_sprite->m_tintColor = RGBA::RED;
        break;
    case PlasmaBall::STRAIGHT:
    case PlasmaBall::NUM_BEHAVIORS:
    default:
        m_sprite->m_tintColor = RGBA::GREEN;
        break;
    }
    //m_sprite->m_tintColor = ((Ship*)owner)->m_factionColor;
    m_sprite->m_tintColor.SetAlphaFloat(0.2f);
    CalculateCollisionRadius();

    m_trail = new RibbonParticleSystem("BeamTrail", TheGame::BACKGROUND_PARTICLES_LAYER, Transform2D(), &m_transform);
    m_trail->m_colorOverride = m_sprite->m_tintColor;
    SetPosition(owner->GetMuzzlePosition());
    m_centralPosition = GetPosition();

    float parentRotationDegrees = m_owner->m_transform.GetWorldRotationDegrees();
    float totalRotationDegrees = parentRotationDegrees + degreesOffset;
    m_transform.SetRotationDegrees(totalRotationDegrees);
    m_muzzleDirection = Vector2::DegreesToDirection(-totalRotationDegrees, Vector2::ZERO_DEGREES_UP);

    float ownerForwardSpeed = Vector2::Dot(m_muzzleDirection, m_owner->m_velocity);
    ownerForwardSpeed = std::max<float>(0.0f, ownerForwardSpeed);
    float adjustedSpeed = m_speed + ownerForwardSpeed;

    Vector2 muzzleVelocity = m_muzzleDirection * adjustedSpeed;
    m_velocity = muzzleVelocity;
    m_centralVelocity = muzzleVelocity;
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
    static const float SPRITE_ANGULAR_VELOCITY = 360.0f;
    static const float AMPLITUDE = 2.0f;
    static const float AGE_OFFSET = 1.0f;
    const float WAVE_DEGREES_PER_SECOND = (m_behavior == PlasmaBall::STRAIGHT) ? 0.0f : 540.0f;

    float newRotationDegrees = m_transform.GetWorldRotationDegrees() + (SPRITE_ANGULAR_VELOCITY * deltaSeconds);
    m_transform.SetRotationDegrees(newRotationDegrees);

    float mass = 1.0f;
    Entity::Update(deltaSeconds);
    if (m_age < m_lifeSpan)
    {
        m_centralPosition += m_centralVelocity * deltaSeconds; //Advance non-wavy position
        m_wavePhase += (WAVE_DEGREES_PER_SECOND * deltaSeconds); //Advance the wave phase shift, where we are in the wave
        Vector2 forwardDirection = m_centralVelocity.GetNorm();
        Vector2 leftDirection = Vector2(-forwardDirection.y, forwardDirection.x);
        float waveAmplitude = (AMPLITUDE / (m_age + AGE_OFFSET));
        float waveHeight = MathUtils::SinDegrees(m_wavePhase) * waveAmplitude; //Calculate how far to the left our offset is.
        Vector2 waveMotionDisplacement = waveHeight * leftDirection;
        Vector2 newPosition = m_centralPosition + waveMotionDisplacement; //Calculate our actual position, with the new wavy offset
        m_velocity = (newPosition - GetPosition()) / deltaSeconds;
        SetPosition(newPosition);

        //m_transform.SetRotationDegrees(-m_centralVelocity.CalculateThetaDegrees() + 90.0f);
    }
    else
    {
        m_isDead = true;
    }
}
