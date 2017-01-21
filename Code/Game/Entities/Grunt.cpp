#include "Game/Entities/Grunt.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../Items/PowerUp.hpp"
#include "Game/Pilots/Pilot.hpp"

const float Grunt::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
Grunt::Grunt(const Vector2& initialPosition)
    : Ship()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("GreenEnemy", TheGame::ENEMY_LAYER);
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_sprite->m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_baseStats.topSpeed = MathUtils::GetRandomFloatFromZeroTo(1.0f);
    m_baseStats.rateOfFire = 1.5f;
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
    float degrees = GetRotation() + m_angularVelocity * deltaSeconds;
    SetRotation(degrees);

    Vector2 direction = Vector2::DegreesToDirection(-m_sprite->m_transform.GetWorldRotationDegrees(), Vector2::ZERO_DEGREES_UP);
    Vector2 deltaVelocity = direction * m_baseStats.topSpeed * deltaSeconds;
    SetPosition(GetPosition() + deltaVelocity);
    if (m_secondsSinceLastFiredWeapon > m_baseStats.rateOfFire)
    {
        TheGame::instance->m_currentGameMode->SpawnBullet(this);
        m_secondsSinceLastFiredWeapon = 0.0f;
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
    if (MathUtils::CoinFlip())
    {
        TheGame::instance->m_currentGameMode->SpawnPickup(new PowerUp(), GetPosition());
    }
}
