#pragma once
#include "Game/Items/Chassis/Chassis.hpp"

class Ship;

class TankChassis : public Chassis
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    TankChassis();
    virtual ~TankChassis();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual const SpriteResource* GetShipSpriteResource();
    virtual void Activate(NamedProperties& props) override;
    virtual void Deactivate(NamedProperties& props) override;

    static constexpr float COLLISION_DAMAGE_PER_COLLISION = 5.0f;
    Ship* m_owner;
};


