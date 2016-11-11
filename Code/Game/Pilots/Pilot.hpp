#pragma once
#include "Engine\Input\InputMap.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"

class Pilot
{
public:
    Pilot();
    virtual ~Pilot();

    virtual void Update(float deltaSeconds) { UNUSED(deltaSeconds); };

    InputMap m_inputMap;
};