#pragma once
#include "Game/Items/Chassis/Chassis.hpp"

class BlackHoleChassis : public Chassis
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    BlackHoleChassis();
    virtual ~BlackHoleChassis();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual const SpriteResource* GetShipSpriteResource();
};
