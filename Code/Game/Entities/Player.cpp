#include "Game/Entities/Player.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XInputController.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Game/TheGame.hpp"

//-----------------------------------------------------------------------------------
Player::Player()
    : Ship()
    , m_netOwnerIndex(0)
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
    if (this != TheGame::instance->m_localPlayer)
    {
        return;
    }
    Ship::Update(deltaSeconds);
    if (InputSystem::instance->m_controllers[TheGame::instance->m_debuggingControllerIndex]->IsConnected())
    {
        float adjustedSpeed = m_speed / 15.0f;
        Vector2 attemptedPosition = m_sprite->m_position + InputSystem::instance->m_controllers[TheGame::instance->m_debuggingControllerIndex]->GetLeftStickPosition() * adjustedSpeed;
        //TODO: Bounds check
        m_sprite->m_position = attemptedPosition;
        if (InputSystem::instance->m_controllers[TheGame::instance->m_debuggingControllerIndex]->IsRightStickPastDeadzone())
        {
            m_sprite->m_rotationDegrees = InputSystem::instance->m_controllers[TheGame::instance->m_debuggingControllerIndex]->GetRightStickAngleDegrees();
            if (m_timeSinceLastShot > m_rateOfFire)
            {
                TheGame::instance->SpawnBullet(this);
                m_timeSinceLastShot = 0.0f;
            }
        }
        else if (InputSystem::instance->m_controllers[TheGame::instance->m_debuggingControllerIndex]->IsLeftStickPastDeadzone())
        {
            m_sprite->m_rotationDegrees = InputSystem::instance->m_controllers[TheGame::instance->m_debuggingControllerIndex]->GetLeftStickAngleDegrees();
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

