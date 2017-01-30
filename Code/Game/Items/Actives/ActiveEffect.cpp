#include "Game/Items/Actives/ActiveEffect.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"

//-----------------------------------------------------------------------------------
ActiveEffect::ActiveEffect()
    : Item(ItemType::ACTIVE)
{
    m_equipText = "Hold A to Equip";
}

//-----------------------------------------------------------------------------------
ActiveEffect::~ActiveEffect()
{

}

//-----------------------------------------------------------------------------------
const SpriteResource* ActiveEffect::GetSpriteResource()
{
    return nullptr;
}

//-----------------------------------------------------------------------------------
void ActiveEffect::Cooldown(float deltaSeconds)
{
    m_energy += (m_energyRestorationPerSecond * deltaSeconds);
    m_energy = Clamp<float>(m_energy, 0.0f, 1.0f);
}
