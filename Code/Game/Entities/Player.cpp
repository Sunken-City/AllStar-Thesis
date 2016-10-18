#include "Game/Entities/Player.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/InputValues.hpp"
#include "Engine/Input/XInputController.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Input/Logging.hpp"

//-----------------------------------------------------------------------------------
Player::Player()
    : Ship()
{
    m_isDead = false;
    m_maxHp = 99999999.0f;
    m_hp = 99999999.0f;
    m_sprite = new Sprite("PlayerShip", TheGame::PLAYER_LAYER);
    m_sprite->m_scale = Vector2(0.25f, 0.25f);
    m_speed = 1.0f;
    m_rateOfFire = 0.5f;
}

//-----------------------------------------------------------------------------------
Player::~Player()
{

}

//-----------------------------------------------------------------------------------
void Player::Update(float deltaSeconds)
{
    const float adjustedSpeed = m_speed / 15.0f;
    Ship::Update(deltaSeconds);

    //Poll Input
    InputMap& input = TheGame::instance->m_gameplayMapping;
    Vector2 inputDirection = input.GetVector2("Right", "Up");
    Vector2 shootDirection = input.GetVector2("ShootRight", "ShootUp");
    bool isShooting = input.FindInputValue("Shoot")->IsDown();

    Vector2 attemptedPosition = m_transform.position + inputDirection * adjustedSpeed;
    AttemptMovement(attemptedPosition);

    if (shootDirection != Vector2::ZERO)
    {
        m_sprite->m_rotationDegrees = shootDirection.GetDirectionDegreesFromNormalizedVector();
    }

    if (isShooting)
    {
        if (m_timeSinceLastShot > m_rateOfFire)
        {
            TheGame::instance->SpawnBullet(this);
            m_timeSinceLastShot = 0.0f;
        }
    }
}

//-----------------------------------------------------------------------------------
void Player::Render() const
{

}

//-----------------------------------------------------------------------------------
void Player::ResolveCollision(Entity* otherEntity)
{
    Ship::ResolveCollision(otherEntity);
}

//-----------------------------------------------------------------------------------
void Player::AttemptMovement(const Vector2& attemptedPosition)
{
    //Todo: check for collisions against level geometry

    m_sprite->m_position = attemptedPosition;
    m_transform.position = attemptedPosition;
}

