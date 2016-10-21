#include "Game/Entities/Player.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/InputValues.hpp"
#include "Engine/Input/XInputController.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"
#include "Game/Items/Weapons/Weapon.hpp"
#include "../Items/PowerUp.hpp"

//-----------------------------------------------------------------------------------
Player::Stats::Stats()
    : topSpeed(0)
    , acceleration(0)
    , agility(0)
    , braking(0)
    , damage(0)
    , shieldDisruption(0)
    , shieldPenetration(0)
    , rateOfFire(0)
    , hp(0)
    , shieldCapacity(0)
    , shieldRegen(0)
    , shotDeflection(0)
{

}

//-----------------------------------------------------------------------------------
//Counts only the positive boosts that could be dropped as items.
unsigned int Player::Stats::GetTotalNumberOfDroppablePowerUps()
{
    unsigned int totalCount = 0;
    totalCount += topSpeed > 0 ? topSpeed : 0;
    totalCount += acceleration > 0 ? acceleration : 0;
    totalCount += agility > 0 ? agility : 0;
    totalCount += braking > 0 ? braking : 0;
    totalCount += damage > 0 ? damage : 0;
    totalCount += shieldDisruption > 0 ? shieldDisruption : 0;
    totalCount += shieldPenetration > 0 ? shieldPenetration : 0;
    totalCount += rateOfFire > 0 ? rateOfFire : 0;
    totalCount += hp > 0 ? hp : 0;
    totalCount += shieldCapacity > 0 ? shieldCapacity : 0;
    totalCount += shieldRegen > 0 ? shieldRegen : 0;
    totalCount += shotDeflection > 0 ? shotDeflection : 0;
    return totalCount;
}

//-----------------------------------------------------------------------------------
short* Player::Stats::GetStatReference(PowerUpType type)
{
    switch (type)
    {
    case PowerUpType::TOP_SPEED:
        return &topSpeed;
    case PowerUpType::ACCELERATION:
        return &acceleration;
    case PowerUpType::AGILITY:
        return &agility;
    case PowerUpType::BRAKING:
        return &braking;
    case PowerUpType::DAMAGE:
        return &damage;
    case PowerUpType::SHIELD_DISRUPTION:
        return &shieldDisruption;
    case PowerUpType::SHIELD_PENETRATION:
        return &shieldPenetration;
    case PowerUpType::RATE_OF_FIRE:
        return &rateOfFire;
    case PowerUpType::HP:
        return &hp;
    case PowerUpType::SHIELD_CAPACITY:
        return &shieldCapacity;
    case PowerUpType::SHIELD_REGEN:
        return &shieldRegen;
    case PowerUpType::SHOT_DEFLECTION:
        return &shotDeflection;
    default:
        ERROR_RECOVERABLE("Passed an invalid type to the stat reference function");
    }
}

//-----------------------------------------------------------------------------------
Player::Stats& Player::Stats::operator+=(const Stats& rhs)
{
    this->topSpeed += rhs.topSpeed;
    this->acceleration += rhs.acceleration;
    this->agility += rhs.agility;
    this->braking += rhs.braking;
    this->damage += rhs.damage;
    this->shieldDisruption += rhs.shieldDisruption;
    this->shieldPenetration += rhs.shieldPenetration;
    this->rateOfFire += rhs.rateOfFire;
    this->hp += rhs.hp;
    this->shieldCapacity += rhs.shieldCapacity;
    this->shieldRegen += rhs.shieldRegen;
    this->shotDeflection += rhs.shotDeflection;
    return *this;
}

//-----------------------------------------------------------------------------------
Player::Player()
    : Ship()
    , m_weapon(nullptr)
    , m_chassis(nullptr)
    , m_activeEffect(nullptr)
    , m_passiveEffect(nullptr)
{
    m_isDead = false;
    m_maxHp = 5.0f;
    m_hp = 5.0f;
    m_sprite = new Sprite("PlayerShip", TheGame::PLAYER_LAYER);
    m_sprite->m_scale = Vector2(0.25f, 0.25f);
    m_speed = 1.0f;
    m_rateOfFire = 0.5f;
}

//-----------------------------------------------------------------------------------
Player::~Player()
{
    //Casual reminder that the sprite is deleted on the entity
}

//-----------------------------------------------------------------------------------
void Player::Update(float deltaSeconds)
{
    const float adjustedSpeed = m_speed / 15.0f;
    Ship::Update(deltaSeconds);

    //Poll Input
    InputMap& input = TheGame::instance->m_gameplayMapping;
    Vector2 inputDirection = input.GetVector2("Right", "Up");
    Vector2 shootDirection = input.GetVector2("ShootRight", "ShootUp");
    bool isShooting = input.FindInputValue("Shoot")->IsDown();

    Vector2 attemptedPosition = m_transform.position + inputDirection * adjustedSpeed;
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
            if (m_timeSinceLastShot > m_rateOfFire)
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
void Player::Render() const
{

}

//-----------------------------------------------------------------------------------
void Player::ResolveCollision(Entity* otherEntity)
{
    Ship::ResolveCollision(otherEntity);
}

//-----------------------------------------------------------------------------------
void Player::Die()
{
    DropPowerups();
}

//-----------------------------------------------------------------------------------
void Player::DropPowerups()
{
    unsigned int numPowerups = m_stats.GetTotalNumberOfDroppablePowerUps();
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
void Player::DropRandomPowerup()
{
    PowerUpType type;
    short* statValue = nullptr;
    do 
    {
        type = static_cast<PowerUpType>(MathUtils::GetRandomIntFromZeroTo((int)PowerUpType::HYBRID));
        statValue = m_stats.GetStatReference(type);
    } while (*statValue < 1);

    TheGame::instance->SpawnPickup(new PowerUp(type), m_transform.position);
    *statValue -= 1;
}

//-----------------------------------------------------------------------------------
void Player::AttemptMovement(const Vector2& attemptedPosition)
{
    //Todo: check for collisions against level geometry
    m_sprite->m_position = attemptedPosition;
    m_transform.position = attemptedPosition;
}

//-----------------------------------------------------------------------------------
//The player now has ownership of this item pointer, and is responsible for cleanup.
void Player::PickUpItem(Item* pickedUpItem)
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
}
