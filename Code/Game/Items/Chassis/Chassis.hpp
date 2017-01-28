#pragma once
#include "Game/Items/Item.hpp"

class Chassis : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Chassis();
    virtual ~Chassis();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource() = 0;
    virtual const SpriteResource* GetShipSpriteResource() = 0;
};