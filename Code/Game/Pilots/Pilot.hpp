#pragma once
#include "Engine\Input\InputMap.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
class Ship;

class Pilot
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Pilot();
    virtual ~Pilot();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds, Ship* currentShip) { UNUSED(deltaSeconds); UNUSED(currentShip); };
    virtual void RecoilScreenshake(float magnitude, const Vector2& direction);
    virtual void LightRumble(float amount, float secondsDuration = 0.25f);
    virtual void HeavyRumble(float amount, float secondsDuration = 0.25f);

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    InputMap m_inputMap;
};