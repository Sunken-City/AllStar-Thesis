#include "Game/Entities/Enemies/Grunt.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"

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

    m_baseStats.topSpeed = MathUtils::GetRandomFloatFromZeroTo(1.0f);
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

    float degrees = GetRotation() + m_angularVelocity * deltaSeconds;
    SetRotation(degrees);

    Vector2 direction = Vector2::DegreesToDirection(-m_transform.GetWorldRotationDegrees(), Vector2::ZERO_DEGREES_UP);
    Vector2 velocity = direction * m_baseStats.topSpeed;

    Vector2 pos = m_transform.GetWorldPosition();
    pos += (m_velocity + velocity) * deltaSeconds;
    m_velocity *= 0.9f;
    SetPosition(pos);

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
