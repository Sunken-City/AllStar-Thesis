#pragma once
#include "Game/Items/Item.hpp"

//-----------------------------------------------------------------------------------
enum PowerUpTypes
{
    TOP_SPEED,
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
    NUM_POWERUP_TYPES
};

//-----------------------------------------------------------------------------------
class PowerUp : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    PowerUp();
    virtual ~PowerUp();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource() override;
    const char* GetPowerUpSpriteResourceName();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    PowerUpTypes m_type;
};