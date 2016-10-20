#include "Game/Items/Actives/Actives.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"

//-----------------------------------------------------------------------------------
Active::Active()
    : Item(ItemType::ACTIVE)
{

}

//-----------------------------------------------------------------------------------
Active::~Active()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* Active::GetSpriteResource()
{
    return nullptr;
}
