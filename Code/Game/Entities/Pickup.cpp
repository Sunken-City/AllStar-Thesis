#include "Game/Entities/Pickup.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "PlayerShip.hpp"
#include "Engine/Renderer/2D/TextRenderable2D.hpp"

//-----------------------------------------------------------------------------------
Pickup::Pickup(Item* item, const Vector2& initialPosition)
    : Entity()
    , m_item(item)
    , m_descriptionTextRenderable(new TextRenderable2D(item->m_name, Transform2D(Vector2(0.0f, 0.9f), 0.0f, Vector2::ONE, &m_transform), TheGame::ITEM_TEXT_LAYER))
    , m_equipTextRenderable(new TextRenderable2D(item->m_equipText, Transform2D(Vector2(0.0f, 0.4f), 0.0f, Vector2::ONE, &m_transform), TheGame::ITEM_TEXT_LAYER))
{
    ASSERT_OR_DIE(m_item, "Attempted to create a pickup with no item!");
    m_collidesWithBullets = false;
    m_noCollide = true;
    m_sprite = new Sprite("Invalid", TheGame::ITEM_LAYER);
    m_sprite->m_spriteResource = m_item->GetSpriteResource();
    CalculateCollisionRadius();

    float x = MathUtils::GetRandomIntFromZeroTo(2) == 1 ? MathUtils::GetRandomFloatFromZeroTo(1.0f) : -MathUtils::GetRandomFloatFromZeroTo(1.0f);
    float y = MathUtils::GetRandomIntFromZeroTo(2) == 1 ? MathUtils::GetRandomFloatFromZeroTo(1.0f) : -MathUtils::GetRandomFloatFromZeroTo(1.0f);
    SetPosition(initialPosition + Vector2(x, y));
    m_sprite->m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(15.0f));
    m_baseStats.hp = 10.0f;
    m_currentHp = m_baseStats.hp;

    float directionDegrees = MathUtils::GetRandomFloatFromZeroTo(360.0f);
    m_velocity = Vector2::DegreesToDirection(directionDegrees) * 10.0f;

    if (!m_item->IsPowerUp())
    {
        m_transform.AddChild(&m_descriptionTextRenderable->m_transform);
        m_descriptionTextRenderable->m_fontSize = 0.05f;
        m_descriptionTextRenderable->Disable();

        m_transform.AddChild(&m_equipTextRenderable->m_transform);
        m_equipTextRenderable->m_fontSize = 0.03f;
        m_equipTextRenderable->Disable();
    }
    else
    {
        m_descriptionTextRenderable->Disable();
        m_equipTextRenderable->Disable();
    }
}

//-----------------------------------------------------------------------------------
Pickup::~Pickup()
{
    if (m_item)
    {
        delete m_item;
    }
    delete m_equipTextRenderable;
    delete m_descriptionTextRenderable;
}

//-----------------------------------------------------------------------------------
void Pickup::Update(float deltaSeconds)
{
    const float FLASH_AGE_SECONDS = m_maxAge - 5.0f;
    Entity::Update(deltaSeconds);

    Vector2 newScale = Vector2(1.0f) + Vector2(sin(m_age * 2.0f) / 4.0f);
    m_sprite->m_transform.SetScale(newScale);
    m_velocity *= 0.9f;
    Vector2 attemptedPosition = GetPosition() + (m_velocity * deltaSeconds);
    SetPosition(attemptedPosition);

    m_descriptionTextRenderable->Disable();
    m_equipTextRenderable->Disable();

    if (m_age > FLASH_AGE_SECONDS)
    {
        float alphaValue = static_cast<int>(m_age * 10.0f) % 2 == 0 ? 1.0f : 0.0f;
        m_sprite->m_tintColor.SetAlphaFloat(alphaValue);
    }
    if (m_age > m_maxAge)
    {
        m_isDead = true;
    }
}

//-----------------------------------------------------------------------------------
void Pickup::Render() const
{

}

//-----------------------------------------------------------------------------------
void Pickup::ResolveCollision(Entity* otherEntity)
{
    if (m_age < 0.05f)
    {
        return;
    }

    for (PlayerShip* player : TheGame::instance->m_players)
    {
        if ((Entity*)player == otherEntity && !player->m_isDead)
        {
            if (m_item && player->CanPickUp(m_item))
            {
                player->PickUpItem(m_item);
                m_item = nullptr;
                this->m_isDead = true;
            }
            else
            {
                m_descriptionTextRenderable->Enable();
                m_equipTextRenderable->Enable();
            }
        }
    }
}
