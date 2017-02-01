#include "Game/Items/PowerUp.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/GameModes/GameMode.hpp"
#include "Game/Entities/PlayerShip.hpp"
#include <string>

//-----------------------------------------------------------------------------------
PowerUp::PowerUp(PowerUpType type)
    : Item(ItemType::POWER_UP)
    , m_powerUpType(type)
{
    m_name = "Pickup";
    if (m_powerUpType == PowerUpType::RANDOM)
    {
        m_powerUpType = static_cast<PowerUpType>(MathUtils::GetRandomIntFromZeroTo((int)PowerUpType::NUM_POWERUP_TYPES));
    }
    SetStatChangeFromType(m_powerUpType);
}

//-----------------------------------------------------------------------------------
PowerUp::~PowerUp()
{

}

//-----------------------------------------------------------------------------------
void PowerUp::SetStatChangeFromType(PowerUpType type)
{
    switch (type)
    {
    case PowerUpType::TOP_SPEED:
        m_statChanges.topSpeed = 1;
        break;
    case PowerUpType::ACCELERATION:
        m_statChanges.acceleration = 1;
        break;
    case PowerUpType::HANDLING:
        m_statChanges.handling = 1;
        break;
    case PowerUpType::BRAKING:
        m_statChanges.braking = 1;
        break;
    case PowerUpType::DAMAGE:
        m_statChanges.damage = 1;
        break;
    case PowerUpType::SHIELD_DISRUPTION:
        m_statChanges.shieldDisruption = 1;
        break;
    case PowerUpType::SHOT_HOMING:
        m_statChanges.shotHoming = 1;
        break;
    case PowerUpType::RATE_OF_FIRE:
        m_statChanges.rateOfFire = 1;
        break;
    case PowerUpType::HP:
        m_statChanges.hp = 1;
        break;
    case PowerUpType::SHIELD_CAPACITY:
        m_statChanges.shieldCapacity = 1;
        break;
    case PowerUpType::SHIELD_REGEN:
        m_statChanges.shieldRegen = 1;
        break;
    case PowerUpType::SHOT_DEFLECTION:
        m_statChanges.shotDeflection = 1;
        break;
    default:
        ERROR_RECOVERABLE("Invalid PowerUpType used for setting stats");
    }
}

//-----------------------------------------------------------------------------------
SoundID PowerUp::GetPickupSFXID()
{
    static SoundID pickupSounds[12];
    pickupSounds[0] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_01.wav");
    pickupSounds[1] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_02.wav");
    pickupSounds[2] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_03.wav");
    pickupSounds[3] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_04.wav");
    pickupSounds[4] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_05.wav");
    pickupSounds[5] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_01.wav");
    pickupSounds[6] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_02.wav");
    pickupSounds[7] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_03.wav");
    pickupSounds[8] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_04.wav");
    pickupSounds[9] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_05.wav");
    pickupSounds[10] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_01.wav");
    pickupSounds[11] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_02.wav");
    pickupSounds[12] = AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_03.wav");
    
    return pickupSounds[static_cast<int>(m_powerUpType)];
}

//-----------------------------------------------------------------------------------
const SpriteResource* PowerUp::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource(std::string(GetPowerUpSpriteResourceName()));
}

//-----------------------------------------------------------------------------------
const char* PowerUp::GetPowerUpSpriteResourceName()
{
    return PowerUp::GetPowerUpSpriteResourceName(m_powerUpType);
}

//-----------------------------------------------------------------------------------
const char* PowerUp::GetPowerUpSpriteResourceName(PowerUpType type)
{
    switch (type)
    {
    case PowerUpType::TOP_SPEED:
        return "Top Speed";
    case PowerUpType::ACCELERATION:
        return "Acceleration";
    case PowerUpType::HANDLING:
        return "Handling";
    case PowerUpType::BRAKING:
        return "Braking";
    case PowerUpType::DAMAGE:
        return "Damage";
    case PowerUpType::SHIELD_DISRUPTION:
        return "Shield Disruption";
    case PowerUpType::SHOT_HOMING:
        return "Shot Homing";
    case PowerUpType::RATE_OF_FIRE:
        return "Rate Of Fire";
    case PowerUpType::HP:
        return "Hp";
    case PowerUpType::SHIELD_CAPACITY:
        return "Shield Capacity";
    case PowerUpType::SHIELD_REGEN:
        return "Shield Regen";
    case PowerUpType::SHOT_DEFLECTION:
        return "Shot Deflection";
    default:
        ERROR_AND_DIE("Invalid Pickup type");
    }
}

//-----------------------------------------------------------------------------------
void PowerUp::ApplyPickupEffect(PlayerShip* player)
{
    player->m_powerupStatModifiers += m_statChanges;
}

