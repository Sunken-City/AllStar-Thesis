#include "Game/Entities/Enemies/Grunt.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "../PlayerShip.hpp"

const float Grunt::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
Grunt::Grunt(const Vector2& initialPosition)
    : Ship()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Grunt", TheGame::ENEMY_LAYER);
    m_shieldSprite->m_material = m_sprite->m_material;
    m_shipTrail->m_emitters[0]->m_materialOverride = m_sprite->m_material;

    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_transform.SetScale(Vector2(4.0f));  
    CalculateCollisionRadius();
    SetPosition(initialPosition);

    m_baseStats.topSpeed = 2.0f;
    m_baseStats.rateOfFire -= 5.0f;
    m_baseStats.hp -= 2.0f;
    m_baseStats.shieldCapacity -= 2.0f; 
    SetShieldHealth();
    Heal();
}

//-----------------------------------------------------------------------------------
Grunt::~Grunt()
{
    if (m_pilot)
    {
        delete m_pilot;
    }
}

//-----------------------------------------------------------------------------------
void Grunt::Update(float deltaSeconds)
{
    Ship::Update(deltaSeconds);

    m_timeSinceRetargetSeconds += deltaSeconds;

    if (m_timeSinceRetargetSeconds > TIME_IN_BETWEEN_TARGETING_SECONDS)
    {
        FindTarget();
        m_timeSinceRetargetSeconds = 0.0f;
    }

    if (m_currentTarget)
    {
        Vector2 deltaPosition = m_currentTarget->GetPosition() - GetPosition();
        Vector2 direction = deltaPosition.GetNorm();

        float currRotation = GetRotation();
        float angleDifference = MathUtils::CalcShortestAngularDisplacement(currRotation, deltaPosition.GetDirectionDegrees());
        float desiredRotation = currRotation + angleDifference;
        SetRotation(MathUtils::Lerp(0.10f, currRotation, desiredRotation));

        Vector2 velocity = direction * m_baseStats.topSpeed * 1.25f;

        Vector2 pos = m_transform.GetWorldPosition();
        pos += (m_velocity + velocity) * deltaSeconds;
        m_velocity *= 0.9f;
        SetPosition(pos);
    }
    else
    {
        float degrees = GetRotation() + m_angularVelocity * deltaSeconds;
        SetRotation(degrees);

        Vector2 direction = Vector2::DegreesToDirection(-m_transform.GetWorldRotationDegrees(), Vector2::ZERO_DEGREES_UP);
        Vector2 velocity = direction * m_baseStats.topSpeed;

        Vector2 pos = m_transform.GetWorldPosition();
        pos += (m_velocity + velocity) * deltaSeconds;
        m_velocity *= 0.9f;
        SetPosition(pos);
    }

    if (m_weapon)
    {
        m_weapon->AttemptFire(this);
    }
    else
    {
        m_defaultWeapon.AttemptFire(this);
    }
}

//-----------------------------------------------------------------------------------
void Grunt::Render() const
{

}

//-----------------------------------------------------------------------------------
void Grunt::Die()
{
    Ship::Die();
    TheGame::instance->m_currentGameMode->SpawnPickup(new PowerUp(), GetPosition());
    if (MathUtils::CoinFlip())
    {
        TheGame::instance->m_currentGameMode->SpawnPickup(new PowerUp(), GetPosition());
        if (MathUtils::CoinFlip())
        {
            TheGame::instance->m_currentGameMode->SpawnPickup(new PowerUp(), GetPosition());
        }
    }
}

void Grunt::FindTarget()
{
    m_currentTarget = nullptr;
    float bestDistSquared = 9999999.0f;
    for (PlayerShip* player : TheGame::instance->m_players)
    {
        float distSquared = MathUtils::CalcDistSquaredBetweenPoints(player->GetPosition(), GetPosition());
        float detectionRadius = DETECTION_RADIUS_SQUARED * (MAX_STEALTH_FACTOR - player->m_stealthFactor);
        if (distSquared < detectionRadius && distSquared < bestDistSquared)
        {
            bestDistSquared = distSquared;
            m_currentTarget = player;
        }
    }
}