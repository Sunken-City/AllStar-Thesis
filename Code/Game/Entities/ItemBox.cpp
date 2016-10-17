#include "Game/Entities/ItemBox.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/StateMachine.hpp"

const float ItemBox::MAX_ANGULAR_VELOCITY = 15.0f;

//-----------------------------------------------------------------------------------
ItemBox::ItemBox(const Vector2& initialPosition) 
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("ItemBox", TheGame::ENEMY_LAYER);
    m_sprite->m_scale = Vector2(0.25f, 0.25f);
    m_sprite->m_position = initialPosition;
    m_sprite->m_rotationDegrees = MathUtils::GetRandomFloatFromZeroTo(15.0f);
}

//-----------------------------------------------------------------------------------
ItemBox::~ItemBox()
{
    if (GetGameState() == GameState::PLAYING)
    {
        int maxNumItems = MathUtils::GetRandomIntFromZeroTo(5);
        for (int i = 0; i < maxNumItems; ++i)
        {
            TheGame::instance->SpawnPickup(m_sprite->m_position);
        }
    }
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
