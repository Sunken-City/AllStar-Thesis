#include "Game/Pilots/PlayerPilot.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XInputController.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"

//-----------------------------------------------------------------------------------
PlayerPilot::PlayerPilot(int playerNumber)
    : m_playerNumber(playerNumber)
{

}

//-----------------------------------------------------------------------------------
PlayerPilot::~PlayerPilot()
{

}

//-----------------------------------------------------------------------------------
void PlayerPilot::LightRumble(float amount, float seconds)
{
    if (m_controllerIndex > -1)
    {
        InputSystem::instance->m_controllers[m_controllerIndex]->VibrateForSeconds(seconds, XInputController::NO_VIBRATE, MathUtils::LerpInt(amount, XInputController::NO_VIBRATE, XInputController::MAX_VIBRATE));
    }
    SpriteGameRenderer::instance->AddScreenshakeMagnitude(0.01f, m_playerNumber);
}

//-----------------------------------------------------------------------------------
void PlayerPilot::HeavyRumble(float amount, float seconds)
{
    if (m_controllerIndex > -1)
    {
        InputSystem::instance->m_controllers[m_controllerIndex]->VibrateForSeconds(seconds, MathUtils::LerpInt(amount, XInputController::NO_VIBRATE, XInputController::MAX_VIBRATE), XInputController::NO_VIBRATE);
    }

    SpriteGameRenderer::instance->AddScreenshakeMagnitude(MathUtils::Lerp(amount, 0.05f, 0.15f), m_playerNumber);
}
