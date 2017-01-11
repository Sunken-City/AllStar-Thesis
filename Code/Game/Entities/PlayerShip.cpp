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
#include "Game/Pilots/PlayerPilot.hpp"
#include "Engine/Audio/Audio.hpp"

//-----------------------------------------------------------------------------------
PlayerShip::PlayerShip(PlayerPilot* pilot)
    : Ship((Pilot*)pilot)
{
    m_isDead = false;
    m_sprite = new Sprite("PlayerShip", TheGame::PLAYER_LAYER);
    m_sprite->m_tintColor = RGBA::GetRandom(); 
    m_sprite->m_scale = Vector2(0.25f, 0.25f);
    CalculateCollisionRadius();

    m_baseStats.hp = 5.0f;
    m_currentHp = m_baseStats.hp;
    m_baseStats.acceleration = 1.0f;
    m_baseStats.handling = 1.0f;
    m_baseStats.topSpeed = 1.0f;
    m_baseStats.rateOfFire = 8.0f;

    m_hitSoundMaxVolume = 1.0f;
    m_shieldSprite->m_tintColor = m_sprite->m_tintColor;
}

//-----------------------------------------------------------------------------------
PlayerShip::~PlayerShip()
{
    //Casual reminder that the sprite is deleted on the entity
}

//-----------------------------------------------------------------------------------
void PlayerShip::Update(float deltaSeconds)
{
    if (m_isDead && m_pilot->m_inputMap.FindInputValue("Respawn")->WasJustPressed())
    {
        Respawn();
    }
    if (!m_isDead)
    {
        Ship::Update(deltaSeconds);
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
    Ship::Die();
    DropPowerups();
    m_velocity = Vector2::ZERO;
    m_sprite->Disable();
}

//-----------------------------------------------------------------------------------
void PlayerShip::Respawn()
{
    m_isDead = false;
    Heal(CalculateHpValue());
    SetShieldHealth(CalculateShieldCapacityValue());
    m_sprite->Enable();
    SetPosition(TheGame::instance->m_currentGameMode->GetRandomPlayerSpawnPoint());
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

    TheGame::instance->m_currentGameMode->SpawnPickup(new PowerUp(type), m_transform.position);
    *statValue -= 1.0f;
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
        PowerUp* powerUp = ((PowerUp*)pickedUpItem);
        powerUp->ApplyPickupEffect(this);

        GameMode::GetCurrent()->PlaySoundAt(powerUp->GetPickupSFXID(), GetPosition());
        ParticleSystem::PlayOneShotParticleEffect("PowerupPickup", TheGame::PLAYER_LAYER, GetPosition(), 0.0f, powerUp->GetSpriteResource());

        delete powerUp;
    }
    if (pickedUpItem->IsChassis())
    {
        //Eject(m_chassis);
        m_chassis = (Chassis*)pickedUpItem;
    }
}
