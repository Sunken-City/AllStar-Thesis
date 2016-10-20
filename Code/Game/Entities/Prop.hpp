#pragma once
#include "Game/Entities/Entity.hpp"

class Item;

class Prop : public Entity
{
public:
    Prop();
    virtual ~Prop();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void DropInventory();
    void InitializeInventory(unsigned int inventorySize);
    void DeleteInventory();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Item** m_inventory;
    unsigned int m_inventorySize;
};