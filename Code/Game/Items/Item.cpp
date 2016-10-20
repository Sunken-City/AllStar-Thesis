#include "Game/Items/Item.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
Item::Item(ItemType type)
    : m_itemType(type)
{

}

//-----------------------------------------------------------------------------------
Item::~Item()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* Item::GetSpriteResource()
{
    return ResourceDatabase::instance->GetSpriteResource("Invalid");
}
