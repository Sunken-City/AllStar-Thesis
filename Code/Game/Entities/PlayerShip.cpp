#include "Game/Entities/PlayerShip.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/InputValues.hpp"
#include "Engine/Input/XInputController.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "Game/Items/PowerUp.hpp"

//-----------------------------------------------------------------------------------
PlayerShip::PlayerShip()
    : Ship()
{
    m_isDead = false;
    m_maxHp = 5.0f;
    m_hp = 5.0f;
    m_sprite = new Sprite("PlayerShip", TheGame::PLAYER_LAYER);
    m_sprite->m_scale = Vector2(0.25f, 0.25f);
    m_baseStats.acceleration = 1.0f;
    m_baseStats.agility = 1.0f;
    m_baseStats.braking = 0.0f;
    m_baseStats.topSpeed = 1.0f;
    m_baseStats.rateOfFire = 0.5f;
}

//-----------------------------------------------------------------------------------
PlayerShip::~PlayerShip()
{
    //Casual reminder that the sprite is deleted on the entity
}

//-----------------------------------------------------------------------------------
void PlayerShip::Update(float deltaSeconds)
{
    const float speedSanityMultiplier = 1.0f / 15.0f;
    Ship::Update(deltaSeconds);
    
    //Poll Input
    InputMap& input = TheGame::instance->m_gameplayMapping;
    Vector2 inputDirection = input.GetVector2("Right", "Up");
    //Vector2 inputDirection = InputSystem::instance->m_controllers[0]->GetLeftStickPosition();
    Vector2 shootDirection = input.GetVector2("ShootRight", "ShootUp");
    bool isShooting = input.FindInputValue("Shoot")->IsDown();

    //Calculate velocity
    Vector2 velocityDir = m_velocity.CalculateMagnitude() < 0.01f ? inputDirection.GetNorm() : m_velocity.GetNorm();
    Vector2 perpindicularVelocityDir(-velocityDir.y, velocityDir.x);

    float accelerationDot = Vector2::Dot(inputDirection, velocityDir);
    float accelerationMultiplier = (accelerationDot >= 0.0f) ? GetAccelerationStat() : GetBrakingStat();
    Vector2 accelerationComponent = accelerationComponent = velocityDir * accelerationDot * accelerationMultiplier;
    Vector2 agilityComponent = perpindicularVelocityDir * Vector2::Dot(inputDirection, perpindicularVelocityDir) * GetAgilityStat();

    Vector2 totalAcceleration = accelerationComponent + agilityComponent;
    m_velocity += totalAcceleration * deltaSeconds;
    m_velocity *= m_frictionValue;
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

//-----------------------------------------------------------------------------------
void PlayerShip::Render() const
{

}

//-----------------------------------------------------------------------------------
void PlayerShip::ResolveCollision(Entity* otherEntity)
{
    Ship::ResolveCollision(otherEntity);
}

//-----------------------------------------------------------------------------------
void PlayerShip::Die()
{
    DropPowerups();
}

//-----------------------------------------------------------------------------------
void PlayerShip::DropPowerups()
{
    unsigned int numPowerups = m_powerupStatModifiers.GetTotalNumberOfDroppablePowerUps();
    unsigned int numPowerupsToSpawn = 0;
    if (numPowerups <= 3)
    {
        numPowerupsToSpawn = numPowerups;
    }
    else
    {
        numPowerupsToSpawn = 3;
    }
    for (unsigned int i = 0; i < numPowerupsToSpawn; ++i)
    {
        DropRandomPowerup();
    }
}

//-----------------------------------------------------------------------------------
void PlayerShip::DropRandomPowerup()
{
    PowerUpType type;
    float* statValue = nullptr;
    do 
    {
        type = static_cast<PowerUpType>(MathUtils::GetRandomIntFromZeroTo((int)PowerUpType::HYBRID));
        statValue = m_powerupStatModifiers.GetStatReference(type);
    } while (*statValue < 1.0f);

    TheGame::instance->SpawnPickup(new PowerUp(type), m_transform.position);
    *statValue -= 1.0f;
}

//-----------------------------------------------------------------------------------
void PlayerShip::AttemptMovement(const Vector2& attemptedPosition)
{
    //Todo: check for collisions against level geometry
    m_sprite->m_position = attemptedPosition;
    m_transform.position = attemptedPosition;
}

//-----------------------------------------------------------------------------------
//The player now has ownership of this item pointer, and is responsible for cleanup.
void PlayerShip::PickUpItem(Item* pickedUpItem)
{
    if (!pickedUpItem)
    {
        return;
    }
    if (pickedUpItem->IsPowerUp())
    {
        ((PowerUp*)pickedUpItem)->ApplyPickupEffect(this);
        delete pickedUpItem;
    }
    if (pickedUpItem->IsChassis())
    {
        //Eject(m_chassis);
        m_chassis = (Chassis*)pickedUpItem;
    }
}
