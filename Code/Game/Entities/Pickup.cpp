#include "Game/Entities/Pickup.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Player.hpp"

//-----------------------------------------------------------------------------------
Pickup::Pickup(const Vector2& initialPosition)
    : Entity()
    , m_type((PickupType)MathUtils::GetRandomIntFromZeroTo((int)NUM_TYPES))
{
    switch (m_type)
    {
    case SPEED:
        m_sprite = new Sprite("Speed", TheGame::ITEM_LAYER);
        break;
    case POWER:
        m_sprite = new Sprite("Power", TheGame::ITEM_LAYER);
        break;
    case DEFENCE:
        m_sprite = new Sprite("Defence", TheGame::ITEM_LAYER);
        break;
    case FIRERATE:
        m_sprite = new Sprite("FireRate", TheGame::ITEM_LAYER);
        break;
    case HP:
        m_sprite = new Sprite("Hp", TheGame::ITEM_LAYER);
        break;
    default:
        break;
    }
    m_sprite->m_scale = Vector2(1.0f);

    float x = MathUtils::GetRandomIntFromZeroTo(2) == 1 ? MathUtils::GetRandomFloatFromZeroTo(1.0f) : -MathUtils::GetRandomFloatFromZeroTo(1.0f);
    float y = MathUtils::GetRandomIntFromZeroTo(2) == 1 ? MathUtils::GetRandomFloatFromZeroTo(1.0f) : -MathUtils::GetRandomFloatFromZeroTo(1.0f);
    m_sprite->m_position = initialPosition + Vector2(x, y);
    m_sprite->m_rotationDegrees = MathUtils::GetRandomFloatFromZeroTo(15.0f);
    m_maxHp = 9999999.0f;
    m_hp = 9999999.0f;
}

//-----------------------------------------------------------------------------------
Pickup::~Pickup()
{
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
    for (Player* ent : TheGame::instance->m_players)
    {
        if ((Entity*)ent == otherEntity)
        {
            switch (m_type)
            {
            case SPEED:
                ent->m_speed += 0.5f;
                break;
            case POWER:
                ent->m_power += 0.5f;
                break;
            case DEFENCE:
                break;
            case FIRERATE:
                ent->m_rateOfFire /= 1.5f;
                break;
            case HP:
                ent->m_hp += 0.5f;
                ent->m_maxHp += 0.5f;
                break;
            default:
                break;
            }
            this->m_isDead = true;
        }
    }
}
