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
        m_statChanges.hp = 1;
        break;
    }
}

//-----------------------------------------------------------------------------------
SoundID PowerUp::GetPickupSFXID()
{
//     int numbers[] = { 3, 4, 5, 6, 67 };
//     const int numInts = sizeof(numbers) / sizeof(numbers[0]);

    static SoundID pickupSounds[] =
    {
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_01.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_02.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_03.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_04.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_05.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_01.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_02.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_03.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_04.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_05.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_01.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_02.wav"),
        AudioSystem::instance->CreateOrGetSound("Data/SFX/Pickups/Powerups/Whoosh_03.wav")
    };
    
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
RGBA PowerUp::GetPowerUpColor(PowerUpType type)
{
    switch (type)
    {
    case PowerUpType::TOP_SPEED:
        return RGBA(0x7DFA92FF);
    case PowerUpType::ACCELERATION:
        return RGBA(0xB4F67BFF);
    case PowerUpType::HANDLING:
        return RGBA(0x6AD89DFF);
    case PowerUpType::BRAKING:
        return RGBA(0X77D76BFF);
    case PowerUpType::DAMAGE:
        return RGBA(0xE668DBFF);
    case PowerUpType::SHIELD_DISRUPTION:
        return RGBA(0xFD8671FF);
    case PowerUpType::SHOT_HOMING:
        return RGBA(0xE66866FF);
    case PowerUpType::RATE_OF_FIRE:
        return RGBA(0xFF7BB2FF);
    case PowerUpType::HP:
        return RGBA(0x06D1FAFF);
    case PowerUpType::SHIELD_CAPACITY:
        return RGBA(0x1358F9FF);
    case PowerUpType::SHIELD_REGEN:
        return RGBA(0x1885E0FF);
    case PowerUpType::SHOT_DEFLECTION:
        return RGBA(0x27E2CCFF);
    default:
        ERROR_AND_DIE("Invalid Pickup type");
    }
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

