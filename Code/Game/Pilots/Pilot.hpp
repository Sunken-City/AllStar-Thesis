#pragma once
#include "Engine\Input\InputMap.hpp"

class Pilot
{
public:
    Pilot();
    virtual ~Pilot();

    virtual void Update(float deltaSeconds) {};

    InputMap m_inputMap;
};