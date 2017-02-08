#include "Game/Pilots/PlayerPilot.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XInputController.hpp"

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
}

//-----------------------------------------------------------------------------------
void PlayerPilot::HeavyRumble(float amount, float seconds)
{
    if (m_controllerIndex > -1)
    {
        InputSystem::instance->m_controllers[m_controllerIndex]->VibrateForSeconds(seconds, MathUtils::LerpInt(amount, XInputController::NO_VIBRATE, XInputController::MAX_VIBRATE), XInputController::NO_VIBRATE);
    }
}
