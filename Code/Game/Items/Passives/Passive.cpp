#include "Game/Items/Passives/Passive.hpp"

//-----------------------------------------------------------------------------------
Passive::Passive()
    : Item(ItemType::PASSIVE)
{

}

//-----------------------------------------------------------------------------------
Passive::~Passive()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* Passive::GetSpriteResource()
{
    return nullptr;
}
