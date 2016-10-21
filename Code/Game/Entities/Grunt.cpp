#include "Game/Entities/Grunt.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../Items/PowerUp.hpp"

const float Grunt::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
Grunt::Grunt(const Vector2& initialPosition)
    : Ship()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("GreenEnemy", TheGame::ENEMY_LAYER);
    m_sprite->m_scale = Vector2(1.0f, 1.0f);
    m_sprite->m_position = initialPosition;
    m_sprite->m_rotationDegrees = MathUtils::GetRandomFloatFromZeroTo(15.0f);
    m_speed = MathUtils::GetRandomFloatFromZeroTo(1.0f);
    m_rateOfFire = 1.5f;
}

//-----------------------------------------------------------------------------------
Grunt::~Grunt()
{
}

//-----------------------------------------------------------------------------------
void Grunt::Update(float deltaSeconds)
{
    Ship::Update(deltaSeconds);
    m_sprite->m_rotationDegrees += m_angularVelocity * deltaSeconds;
    Vector2 direction = Vector2::DegreesToDirection(-m_sprite->m_rotationDegrees, Vector2::ZERO_DEGREES_UP);
    Vector2 deltaVelocity = direction * m_speed * deltaSeconds;
    m_sprite->m_position += deltaVelocity;
    if (m_timeSinceLastShot > m_rateOfFire)
    {
        TheGame::instance->SpawnBullet(this);
        m_timeSinceLastShot = 0.0f;
    }
}

//-----------------------------------------------------------------------------------
void Grunt::Render() const
{

}

//-----------------------------------------------------------------------------------
void Grunt::Die()
{
    if (MathUtils::CoinFlip())
    {
        TheGame::instance->SpawnPickup(new PowerUp(), m_sprite->m_position);
    }
}
