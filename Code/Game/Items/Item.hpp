#pragma once

class SpriteResource;

//-----------------------------------------------------------------------------------
enum ItemType
{
    POWER_UP,
    WEAPON,
    ACTIVE,
    PASSIVE,
    CHASSIS,
    NUM_ITEM_TYPES
};

//-----------------------------------------------------------------------------------
class Item
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Item(ItemType type);
    virtual ~Item();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
    inline bool IsPowerUp() { return m_itemType == POWER_UP; };
    inline bool IsWeapon() { return m_itemType == WEAPON; };
    inline bool IsActiveEffect() { return m_itemType == ACTIVE; };
    inline bool IsPassiveEffect() { return m_itemType == PASSIVE; };
    inline bool IsChassis() { return m_itemType == CHASSIS; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    ItemType m_itemType;
};