#include "Game/Entities/Grunt.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Pilots/Pilot.hpp"

const float Grunt::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
Grunt::Grunt(const Vector2& initialPosition)
    : Ship()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Grunt", TheGame::ENEMY_LAYER);
    m_sprite->m_transform.SetScale(Vector2(2.0f));
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_sprite->m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_baseStats.topSpeed = MathUtils::GetRandomFloatFromZeroTo(1.0f);
    m_baseStats.rateOfFire = (1.5f - Stats::BASE_RATE_OF_FIRE) / Stats::RATE_OF_FIRE_PER_POINT;
    m_baseStats.hp -= 2;
    m_baseStats.shieldCapacity -= 2; 
    SetShieldHealth(100000.0f);
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

    float degrees = GetRotation() + m_angularVelocity * deltaSeconds;
    SetRotation(degrees);

    Vector2 direction = Vector2::DegreesToDirection(-m_sprite->m_transform.GetWorldRotationDegrees(), Vector2::ZERO_DEGREES_UP);
    Vector2 deltaVelocity = direction * m_baseStats.topSpeed * deltaSeconds;
    SetPosition(GetPosition() + deltaVelocity);

    m_weapon->AttemptFire(this);
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
