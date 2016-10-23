#pragma once
#include "Game/Items/Item.hpp"
#include "Game/Entities/PlayerShip.hpp"

//-----------------------------------------------------------------------------------
enum class PowerUpType
{
    TOP_SPEED = 0,
    ACCELERATION,
    AGILITY,
    BRAKING,
    DAMAGE,
    SHIELD_DISRUPTION,
    SHIELD_PENETRATION,
    RATE_OF_FIRE,
    HP,
    SHIELD_CAPACITY,
    SHIELD_REGEN,
    SHOT_DEFLECTION,
    HYBRID,
    NUM_POWERUP_TYPES,
    RANDOM
};

//-----------------------------------------------------------------------------------
class PowerUp : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    PowerUp(PowerUpType type = PowerUpType::RANDOM);
    PowerUp(PlayerShip::Stats statChanges);
    virtual ~PowerUp();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource() override;
    const char* GetPowerUpSpriteResourceName();
    void ApplyPickupEffect(PlayerShip* player);
    void SetStatChangeFromType(PowerUpType type);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    PowerUpType m_powerUpType;
    PlayerShip::Stats m_statChanges;
};