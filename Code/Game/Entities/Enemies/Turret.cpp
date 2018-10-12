#include "Game/Entities/Enemies/Turret.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Items/PowerUp.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Pilots/Pilot.hpp"
#include "Engine/Renderer/2D/ParticleSystem.hpp"
#include "../PlayerShip.hpp"
#include "Game/Items/Weapons/MissileLauncher.hpp"
#include "Game/Pilots/TurretPilot.hpp"

//-----------------------------------------------------------------------------------
Turret::Turret(const Vector2& initialPosition)
    : Ship()
{
    m_pilot = new TurretPilot();
    m_sprite = new Sprite("Turret", TheGame::ENEMY_LAYER);
    m_shieldSprite->m_material = m_sprite->m_material;
    m_shipTrail->m_emitters[0]->m_materialOverride = m_sprite->m_material;

    m_sprite->m_transform.SetParent(&m_transform);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_transform.SetScale(Vector2(4.0f));  
    CalculateCollisionRadius();
    SetPosition(initialPosition);

    m_isImmobile = true;
    m_weapon = new MissileLauncher();

    m_baseStats.topSpeed = 0.0f;
    m_baseStats.acceleration = 0.0f;
    m_baseStats.shieldCapacity = 0.0f;
    m_baseStats.shieldRegen = 0.0f;
    m_baseStats.rateOfFire -= 10.0f;
    m_baseStats.hp += 5.0f;
    SetShieldHealth(0.0f);
    Heal();
}

//-----------------------------------------------------------------------------------
Turret::~Turret()
{
    if (m_pilot)
    {
        delete m_pilot;
    }
}

//-----------------------------------------------------------------------------------
void Turret::Update(float deltaSeconds)
{
    Ship::Update(deltaSeconds);
    SetShieldHealth(0.0f);
}

//-----------------------------------------------------------------------------------
void Turret::Render() const
{

}

//-----------------------------------------------------------------------------------
void Turret::Die()
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