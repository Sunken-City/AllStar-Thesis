#pragma once
#include "Game/Entities/Entity.hpp"

class Item;

class Prop : public Entity
{
public:
    Prop();
    virtual ~Prop();

    Item* m_inventory;
    unsigned int m_inventorySize;
};