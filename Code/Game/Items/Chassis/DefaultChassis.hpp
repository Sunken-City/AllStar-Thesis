#pragma once
#include "Game/Items/Chassis/Chassis.hpp"

class DefaultChassis : public Chassis
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    DefaultChassis();
    virtual ~DefaultChassis();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual const SpriteResource* GetShipSpriteResource();
};
