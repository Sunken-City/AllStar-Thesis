#include "Game/Items/Item.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Engine/Renderer/2D/ResourceDatabase.hpp"

//-----------------------------------------------------------------------------------
Item::Item(ItemType type)
    : m_itemType(type)
    , m_statBonuses(0.0f)
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
