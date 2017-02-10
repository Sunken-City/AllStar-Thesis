#include "Game/Items/Weapons/Weapon.hpp"

//-----------------------------------------------------------------------------------
Weapon::Weapon()
    : Item(ItemType::WEAPON)
{
    m_equipText = "Hold B to Swap";
}

//-----------------------------------------------------------------------------------
Weapon::~Weapon()
{

}