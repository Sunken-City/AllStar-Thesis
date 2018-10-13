#include "Game/Entities/Props/HealingZone.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"
#include "../Ship.hpp"
#include <algorithm>

const float HealingZone::MAX_ANGULAR_VELOCITY = 60.0f;

//-----------------------------------------------------------------------------------
HealingZone::HealingZone(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("HealingZone", TheGame::FOREGROUND_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_isInvincible = true;
    m_noCollide = true;
    m_collidesWithBullets = false;
}

//-----------------------------------------------------------------------------------
HealingZone::~HealingZone()
{

}

//-----------------------------------------------------------------------------------
void HealingZone::Update(float deltaSeconds)
{
    float newRotationDegrees = m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_transform.SetRotationDegrees(newRotationDegrees);
}

//-----------------------------------------------------------------------------------
void HealingZone::ResolveCollision(Entity* otherEntity)
{
    Ship* otherShip = dynamic_cast<Ship*>(otherEntity);
    if (otherShip)
    {
        if (otherShip->HasShield() || otherShip->HasFullHealth())
        {
            otherShip->HealShield(1.0f);
        }
        else
        {
            otherShip->Heal(1.0f);
        }
    }
}
