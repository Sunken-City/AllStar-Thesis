#pragma once
#include "Game/Items/Item.hpp"

class Passive : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Passive();
    virtual ~Passive();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
};