#pragma once
#include "Game/Items/Weapons/Weapon.hpp"

class Ship;

class SpreadShot : public Weapon
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    SpreadShot();
    virtual ~SpreadShot();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    virtual bool AttemptFire(Ship* shooter);
    virtual void Activate(NamedProperties&) override {};
    virtual void Deactivate(NamedProperties&) override {};

    static constexpr float LIFE_SPAN_PER_PROJECTILE = 0.3f;
};

