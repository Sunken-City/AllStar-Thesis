#pragma once
#include "Game/Items/Chassis/Chassis.hpp"

class AttractorChassis : public Chassis
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    AttractorChassis();
    virtual ~AttractorChassis();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual const SpriteResource* GetShipSpriteResource();
    virtual void Activate(NamedProperties&) override {};
    virtual void Deactivate(NamedProperties&) override {};
};
