#include "Game/Entities/Props/Nebula.hpp"
#include "Engine/Renderer/2D/Sprite.hpp"
#include "Game/TheGame.hpp"

const float Nebula::MAX_ANGULAR_VELOCITY = 0.1f;

//-----------------------------------------------------------------------------------
Nebula::Nebula(const Vector2& initialPosition)
    : Entity()
    , m_angularVelocity(MathUtils::GetRandomFloatFromZeroTo(MAX_ANGULAR_VELOCITY) - (MAX_ANGULAR_VELOCITY * 2.0f))
{
    const char* spriteString = MathUtils::GetRandomIntFromZeroTo(2) == 0 ? "Nebula" : "Nebula2";
    m_sprite = new Sprite(spriteString, TheGame::FOREGROUND_LAYER);
    m_sprite->m_transform.SetParent(&m_transform);
    CalculateCollisionRadius();
    SetPosition(initialPosition);
    m_transform.SetRotationDegrees(MathUtils::GetRandomFloatFromZeroTo(360.0f));
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
    float newRotationDegrees = m_transform.GetWorldRotationDegrees() + (m_angularVelocity * deltaSeconds);
    m_transform.SetRotationDegrees(newRotationDegrees);
}
