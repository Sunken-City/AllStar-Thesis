#include "Game/Entities/Props/ItemCrate.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/StateMachine.hpp"
#include "Game/Items/Item.hpp"
#include "Game/Items/PowerUp.hpp"

const float ItemCrate::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
ItemCrate::ItemCrate(const Vector2& initialPosition) 
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("ItemBox", TheGame::ENEMY_LAYER);
    m_sprite->m_scale = Vector2(0.5f);
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_sprite->m_rotationDegrees = MathUtils::GetRandomFloatFromZeroTo(15.0f);

    InitializeInventory(MathUtils::GetRandomIntFromZeroTo(MAX_NUM_PICKUPS_PER_BOX) + 1);
    GenerateItems();
}

//-----------------------------------------------------------------------------------
ItemCrate::~ItemCrate()
{

}

//-----------------------------------------------------------------------------------
void ItemCrate::Update(float deltaSeconds)
{
    m_sprite->m_rotationDegrees += m_angularVelocity * deltaSeconds;
    Vector2 direction = Vector2::DegreesToDirection(-m_sprite->m_rotationDegrees, Vector2::ZERO_DEGREES_UP);
}

//-----------------------------------------------------------------------------------
void ItemCrate::GenerateItems()
{
    unsigned int inventorySize = m_inventory.size();
    for (unsigned int i = 0; i < inventorySize; ++i)
    {
        m_inventory[i] = new PowerUp();
    }
}
