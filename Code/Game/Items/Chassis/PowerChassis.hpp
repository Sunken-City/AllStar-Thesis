#pragma once
#include "Game/Items/Chassis/Chassis.hpp"

class PowerChassis : public Chassis
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    PowerChassis();
    virtual ~PowerChassis();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual const SpriteResource* GetShipSpriteResource();
};


