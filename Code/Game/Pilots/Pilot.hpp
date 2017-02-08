#pragma once
#include "Engine\Input\InputMap.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"

class Pilot
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Pilot();
    virtual ~Pilot();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds) { UNUSED(deltaSeconds); };
    virtual void LightRumble(float amount, float secondsDuration = 0.25f);
    virtual void HeavyRumble(float amount, float secondsDuration = 0.25f);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    InputMap m_inputMap;
};