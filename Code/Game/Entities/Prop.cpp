#include "Game/Entities/Prop.hpp"
#include "Game/StateMachine.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/StateMachine.hpp"
#include "Game/Items/Item.hpp"

//-----------------------------------------------------------------------------------
Prop::Prop()
    : Entity()
    , m_inventory(nullptr)
    , m_inventorySize(0)
{

}

//-----------------------------------------------------------------------------------
Prop::~Prop()
{
    if (GetGameState() == GameState::PLAYING)
    {
        DropInventory();
    }
    else
    {
        DeleteInventory();
    }
    delete m_inventory;
}

//-----------------------------------------------------------------------------------
void Prop::DeleteInventory()
{
    for (unsigned int i = 0; i < m_inventorySize; ++i)
    {
        if (m_inventory[i])
        {
            delete m_inventory[i];
            m_inventory[i] = nullptr;
        }
    }
}

//-----------------------------------------------------------------------------------
void Prop::DropInventory()
{
    for (unsigned int i = 0; i < m_inventorySize; ++i)
    {
        //This transfers ownership of the item to the pickup.
        TheGame::instance->SpawnPickup(m_inventory[i], m_sprite->m_position);
        m_inventory[i] = nullptr;
    }
}

//-----------------------------------------------------------------------------------
void Prop::InitializeInventory(unsigned int inventorySize)
{
    m_inventorySize = inventorySize;
    m_inventory = new Item*[m_inventorySize];
    for (unsigned int i = 0; i < m_inventorySize; ++i)
    {
        m_inventory[i] = nullptr;
    }
}