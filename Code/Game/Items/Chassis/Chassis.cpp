#include "Game/Items/Chassis/Chassis.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"

//-----------------------------------------------------------------------------------
Chassis::Chassis()
    : Item(ItemType::CHASSIS)
{

}

//-----------------------------------------------------------------------------------
Chassis::~Chassis()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* Chassis::GetSpriteResource()
{
    return nullptr;
}
