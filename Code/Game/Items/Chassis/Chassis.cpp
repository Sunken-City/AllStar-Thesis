#include "Game/Items/Chassis/Chassis.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"

//-----------------------------------------------------------------------------------
Chassis::Chassis()
    : Item(ItemType::CHASSIS)
{
    m_equipText = "Hold Y to Equip";
}

//-----------------------------------------------------------------------------------
Chassis::~Chassis()
{

}