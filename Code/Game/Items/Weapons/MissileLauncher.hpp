#pragma once
#include "Game/Items/Weapons/Weapon.hpp"

class Ship;

class MissileLauncher : public Weapon
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    MissileLauncher();
    virtual ~MissileLauncher();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual bool AttemptFire(Ship* shooter);
    virtual void Activate(NamedProperties&) override {};
    virtual void Deactivate(NamedProperties&) override {};
};