#pragma once
#include "Game/Items/Item.hpp"

class Chassis : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Chassis();
    virtual ~Chassis();
    inline virtual const char* GetTypeText() { return "CHASSIS"; };    
    inline virtual RGBA GetTypeColor() { return RGBA::YELLOW; };

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource() = 0;
    virtual const SpriteResource* GetShipSpriteResource() = 0;
};