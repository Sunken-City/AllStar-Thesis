#pragma once
#include "Game/Items/Weapons/Weapon.hpp"

class Ship;

class LeftHookGun : public Weapon
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    LeftHookGun();
    virtual ~LeftHookGun();
    virtual void Activate(NamedProperties&) override {};
    virtual void Deactivate(NamedProperties&) override {};

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual bool AttemptFire(Ship* shooter);
};
