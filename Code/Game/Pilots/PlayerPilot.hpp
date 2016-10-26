#pragma once
#include "Engine\Input\InputMap.hpp"
#include "Game\Pilots\Pilot.hpp"

class PlayerPilot : public Pilot
{
public:
    PlayerPilot();
    virtual ~PlayerPilot();

    virtual void Update(float deltaSeconds) {};
};
