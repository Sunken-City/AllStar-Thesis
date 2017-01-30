#include "Game/Items/Passives/PassiveEffect.hpp"

//-----------------------------------------------------------------------------------
PassiveEffect::PassiveEffect()
    : Item(ItemType::PASSIVE)
{

}

//-----------------------------------------------------------------------------------
PassiveEffect::~PassiveEffect()
{
}

//-----------------------------------------------------------------------------------
const SpriteResource* PassiveEffect::GetSpriteResource()
{
    return nullptr;
}
