#pragma once
#include "Game/Items/Weapons/Weapon.hpp"

class Ship;

class LaserGun : public Weapon
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    LaserGun();
    virtual ~LaserGun();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual bool AttemptFire(Ship* shooter);
};
