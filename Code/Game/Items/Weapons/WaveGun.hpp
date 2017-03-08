#pragma once
#include "Game/Items/Weapons/Weapon.hpp"

class Ship;

class WaveGun : public Weapon
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    WaveGun();
    virtual ~WaveGun();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual bool AttemptFire(Ship* shooter);
    virtual inline float GetKnockbackMagnitude() override { return Weapon::GetKnockbackMagnitude() * 3.0f; };
};


