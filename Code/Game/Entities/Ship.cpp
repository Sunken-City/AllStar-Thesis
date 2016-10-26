#include "Game/Entities/Ship.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Engine/Input/InputMap.hpp"
#include "Engine/Input/InputValues.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/TheGame.hpp"

//-----------------------------------------------------------------------------------
Ship::Ship(Pilot* pilot)
    : Entity()
    , m_timeSinceLastShot(0.0f)
    , m_pilot(pilot)
{

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
        const float speedSanityMultiplier = 1.0f / 15.0f;
        //Poll Input
        InputMap& input = m_pilot->m_inputMap;
        Vector2 inputDirection = input.GetVector2("Right", "Up");
        Vector2 shootDirection = input.GetVector2("ShootRight", "ShootUp");
        bool isShooting = input.FindInputValue("Shoot")->IsDown();

        //Calculate velocity
        Vector2 velocityDir = m_velocity.CalculateMagnitude() < 0.01f ? inputDirection.GetNorm() : m_velocity.GetNorm();
        Vector2 perpindicularVelocityDir(-velocityDir.y, velocityDir.x);

        float accelerationDot = Vector2::Dot(inputDirection, velocityDir);
        float accelerationMultiplier = (accelerationDot >= 0.0f) ? GetAccelerationStat() : GetAgilityStat();
        Vector2 accelerationComponent = accelerationComponent = velocityDir * accelerationDot * accelerationMultiplier;
        Vector2 agilityComponent = perpindicularVelocityDir * Vector2::Dot(inputDirection, perpindicularVelocityDir) * GetAgilityStat();

        Vector2 totalAcceleration = accelerationComponent + agilityComponent;
        m_velocity += totalAcceleration * deltaSeconds;
        m_velocity *= m_frictionValue; // +(0.1f * GetWeightStat());
        m_velocity.ClampMagnitude(GetTopSpeedStat() * speedSanityMultiplier);

        Vector2 attemptedPosition = m_transform.position + m_velocity;
        AttemptMovement(attemptedPosition);

        if (shootDirection != Vector2::ZERO)
        {
            m_sprite->m_rotationDegrees = shootDirection.GetDirectionDegreesFromNormalizedVector();
        }

        if (isShooting)
        {
            if (m_weapon)
            {
                m_weapon->AttemptFire();
            }
            else
            {
                if (m_timeSinceLastShot > m_baseStats.rateOfFire)
                {
                    TheGame::instance->SpawnBullet(this);
                    m_timeSinceLastShot = 0.0f;
                }
            }
        }

        if (input.FindInputValue("Suicide")->WasJustPressed())
        {
            m_isDead = true;
            Die();
        }
    }
}

//-----------------------------------------------------------------------------------
void Ship::Render() const
{

}

//-----------------------------------------------------------------------------------
void Ship::ResolveCollision(Entity* otherEntity)
{
    Entity::ResolveCollision(otherEntity);
}

//-----------------------------------------------------------------------------------
void Ship::AttemptMovement(const Vector2& attemptedPosition)
{
    //Todo: check for collisions against level geometry
    m_sprite->m_position = attemptedPosition;
    m_transform.position = attemptedPosition;
}
