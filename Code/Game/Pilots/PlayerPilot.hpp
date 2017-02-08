#pragma once
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Input\InputMap.hpp"
#include "Game\Pilots\Pilot.hpp"

class PlayerPilot : public Pilot
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    PlayerPilot(int playerNumber = 0);
    virtual ~PlayerPilot();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds) { UNUSED(deltaSeconds); };
    virtual void LightRumble(float amount, float secondsDuration = 0.25f) override;
    virtual void HeavyRumble(float amount, float secondsDuration = 0.25f) override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    int m_playerNumber;
    int m_controllerIndex = -1;
};
