#include "Game/Entities/ItemBox.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/StateMachine.hpp"
#include "../Items/Item.hpp"
#include "../Items/PowerUp.hpp"

const float ItemBox::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
ItemBox::ItemBox(const Vector2& initialPosition) 
    : Prop()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("ItemBox", TheGame::ENEMY_LAYER);
    m_sprite->m_scale = Vector2(0.25f, 0.25f);
    m_sprite->m_position = initialPosition;
    m_sprite->m_rotationDegrees = MathUtils::GetRandomFloatFromZeroTo(15.0f);

    InitializeInventory(MathUtils::GetRandomIntFromZeroTo(MAX_NUM_PICKUPS_PER_BOX) + 1);
    GenerateItems();
}

//-----------------------------------------------------------------------------------
ItemBox::~ItemBox()
{

}

//-----------------------------------------------------------------------------------
void ItemBox::Update(float deltaSeconds)
{
    m_sprite->m_rotationDegrees += m_angularVelocity * deltaSeconds;
    Vector2 direction = Vector2::DegreesToDirection(-m_sprite->m_rotationDegrees, Vector2::ZERO_DEGREES_UP);
}

//-----------------------------------------------------------------------------------
void ItemBox::Render() const
{

}

//-----------------------------------------------------------------------------------
void ItemBox::GenerateItems()
{
    for (unsigned int i = 0; i < m_inventorySize; ++i)
    {
        m_inventory[i] = new PowerUp();
    }
}
