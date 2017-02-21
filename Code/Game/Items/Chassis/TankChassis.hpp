#pragma once
#include "Game/Items/Chassis/Chassis.hpp"

class TankChassis : public Chassis
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    TankChassis();
    virtual ~TankChassis();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual const SpriteResource* GetShipSpriteResource();
};


