#include "Game/Entities/Props/Nebula.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"

const float Nebula::MAX_ANGULAR_VELOCITY = 0.1f;

//-----------------------------------------------------------------------------------
Nebula::Nebula(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    m_sprite = new Sprite("Nebula", TheGame::FOREGROUND_LAYER);
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_sprite->m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
    m_isInvincible = true;
    m_noCollide = true;
    m_collidesWithBullets = false;
}

//-----------------------------------------------------------------------------------
Nebula::~Nebula()
{

}

//-----------------------------------------------------------------------------------
void Nebula::Update(float deltaSeconds)
{
    float newRotationDegrees = m_sprite->m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_sprite->m_transform.SetRotationDegrees(newRotationDegrees);
}
