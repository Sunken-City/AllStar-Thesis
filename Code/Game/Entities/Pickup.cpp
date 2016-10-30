#include "Game/Entities/Pickup.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "PlayerShip.hpp"

//-----------------------------------------------------------------------------------
Pickup::Pickup(Item* item, const Vector2& initialPosition)
    : Entity()
    , m_item(item)
{
    m_sprite = new Sprite("Invalid", TheGame::ITEM_LAYER);
    m_sprite->m_scale = Vector2(1.0f);
    m_sprite->m_spriteResource = m_item->GetSpriteResource();

    float x = MathUtils::GetRandomIntFromZeroTo(2) == 1 ? MathUtils::GetRandomFloatFromZeroTo(1.0f) : -MathUtils::GetRandomFloatFromZeroTo(1.0f);
    float y = MathUtils::GetRandomIntFromZeroTo(2) == 1 ? MathUtils::GetRandomFloatFromZeroTo(1.0f) : -MathUtils::GetRandomFloatFromZeroTo(1.0f);
    m_sprite->m_position = initialPosition + Vector2(x, y);
    m_sprite->m_rotationDegrees = MathUtils::GetRandomFloatFromZeroTo(15.0f);
    m_baseStats.hp = 10.0f;
    m_currentHp = m_baseStats.hp;
}

//-----------------------------------------------------------------------------------
Pickup::~Pickup()
{
    if (m_item)
    {
        delete m_item;
    }
}

//-----------------------------------------------------------------------------------
void Pickup::Update(float deltaSeconds)
{
    Entity::Update(deltaSeconds);
    m_sprite->m_scale = Vector2(1.0f) + Vector2(sin(m_age * 2.0f) / 4.0f);
}

//-----------------------------------------------------------------------------------
void Pickup::Render() const
{

}

//-----------------------------------------------------------------------------------
void Pickup::ResolveCollision(Entity* otherEntity)
{
    Entity::ResolveCollision(otherEntity);
    for (PlayerShip* ent : TheGame::instance->m_currentGameMode->m_players)
    {
        if ((Entity*)ent == otherEntity)
        {
            ent->PickUpItem(m_item);
            m_item = nullptr;
            this->m_isDead = true;
        }
    }
}
