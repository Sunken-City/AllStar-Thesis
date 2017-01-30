#include "Game/Items/Passives/PassiveEffect.hpp"

//-----------------------------------------------------------------------------------
PassiveEffect::PassiveEffect()
    : Item(ItemType::PASSIVE)
{
    m_equipText = "Hold X to Equip";
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
