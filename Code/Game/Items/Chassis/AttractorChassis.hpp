#pragma once
#include "Game/Items/Chassis/Chassis.hpp"

class Ship;

class AttractorChassis : public Chassis
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    AttractorChassis();
    virtual ~AttractorChassis();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual const SpriteResource* GetShipSpriteResource();
    virtual void Activate(NamedProperties& props) override;
    virtual void Deactivate(NamedProperties& props) override;

    Ship* m_owner;
};
