#pragma once
#include "Game/Items/Item.hpp"

class Ship;

class Weapon : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Weapon();
    virtual ~Weapon();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource() = 0;
    virtual bool AttemptFire(Ship* shooter) = 0;
};