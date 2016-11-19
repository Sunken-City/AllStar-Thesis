#include "Game/Entities/Ship.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Engine/Input/InputMap.hpp"
#include "Engine/Input/InputValues.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"

//-----------------------------------------------------------------------------------
Ship::Ship(Pilot* pilot)
    : Entity()
    , m_timeSinceLastShot(0.0f)
    , m_pilot(pilot)
{
    m_baseStats.braking = 0.9f;
}

//-----------------------------------------------------------------------------------
Ship::~Ship()
{

}

//-----------------------------------------------------------------------------------
void Ship::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    m_timeSinceLastShot += deltaSeconds;

    if (m_pilot)
    {
        UpdateMotion(deltaSeconds);
        UpdateShooting();

        if (m_pilot->m_inputMap.FindInputValue("Suicide")->WasJustPressed())
        {
            m_isDead = true;
            Die();
        }
    }
}

//-----------------------------------------------------------------------------------
void Ship::UpdateShooting()
{
    InputMap& input = m_pilot->m_inputMap;
    Vector2 shootDirection = input.GetVector2("ShootRight", "ShootUp");
    bool isShooting = input.FindInputValue("Shoot")->IsDown();
    if (shootDirection != Vector2::ZERO)
    {
        m_sprite->m_rotationDegrees = shootDirection.GetDirectionDegreesFromNormalizedVector();
    }

    if (isShooting && !m_lockMovement)
    {
        if (m_weapon)
        {
            m_weapon->AttemptFire();
        }
        else
        {
            float secondsPerShot = 1.0f / GetRateOfFireStat();
            if (m_timeSinceLastShot > secondsPerShot)
            {
                TheGame::instance->m_currentGameMode->SpawnBullet(this);
                m_timeSinceLastShot = 0.0f;
            }
        }
    }
}

//-----------------------------------------------------------------------------------
void Ship::ResolveCollision(Entity* otherEntity)
{
    Entity::ResolveCollision(otherEntity);
}

//-----------------------------------------------------------------------------------
void Ship::AttemptMovement(const Vector2& attemptedPosition)
{
    if (m_lockMovement)
    {
        return;
    }
    //Todo: check for collisions against level geometry
    SetPosition(attemptedPosition);
}

//-----------------------------------------------------------------------------------
void Ship::UpdateMotion(float deltaSeconds)
{
    const float speedSanityMultiplier = 3.0f / 1.0f;
    InputMap& input = m_pilot->m_inputMap;
    Vector2 inputDirection = input.GetVector2("Right", "Up");

    //Calculate Acceleration components
    Vector2 velocityDir = m_velocity.CalculateMagnitude() < 0.01f ? inputDirection.GetNorm() : m_velocity.GetNorm();
    Vector2 perpindicularVelocityDir(-velocityDir.y, velocityDir.x);
    float accelerationDot = Vector2::Dot(inputDirection, velocityDir);
    float accelerationMultiplier = (accelerationDot >= 0.0f) ? GetAccelerationStat() : GetHandlingStat();
    Vector2 accelerationComponent = accelerationComponent = velocityDir * accelerationDot * accelerationMultiplier;
    Vector2 agilityComponent = perpindicularVelocityDir * Vector2::Dot(inputDirection, perpindicularVelocityDir) * GetHandlingStat();

    //Calculate velocity
    Vector2 totalAcceleration = accelerationComponent + agilityComponent;
    m_velocity += totalAcceleration;
    m_velocity *= m_baseStats.braking; // +(0.1f * GetBrakingStat());
    m_velocity.ClampMagnitude(GetTopSpeedStat() * speedSanityMultiplier);

    Vector2 attemptedPosition = m_transform.position + (m_velocity * deltaSeconds);
    AttemptMovement(attemptedPosition);
}