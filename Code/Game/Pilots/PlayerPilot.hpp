#pragma once
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Input\InputMap.hpp"
#include "Game\Pilots\Pilot.hpp"

class PlayerPilot : public Pilot
{
public:
    PlayerPilot(int playerNumber = 0);
    virtual ~PlayerPilot();

    virtual void Update(float deltaSeconds) { UNUSED(deltaSeconds); };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    int m_playerNumber;
    int m_controllerIndex = -1;
};
