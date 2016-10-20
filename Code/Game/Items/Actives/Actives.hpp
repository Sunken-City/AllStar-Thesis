#pragma once
#include "Game/Items/Item.hpp"

class Actives : public Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Actives();
    virtual ~Actives();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
};