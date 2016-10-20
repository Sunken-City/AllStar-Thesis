#pragma once
#include "Game/Items/Item.hpp"

class Active : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Active();
    virtual ~Active();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
};