#include "Game/Entities/Enemies/Grunt.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "../PlayerShip.hpp"
#include "Game/Pilots/BasicEnemyPilot.hpp"

const float Grunt::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
Grunt::Grunt(const Vector2& initialPosition)
    : Ship()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_pilot = new BasicEnemyPilot();
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