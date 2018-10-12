#pragma once
#include "Engine\Input\InputMap.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "BasicEnemyPilot.hpp"
class Ship;

class TurretPilot : public BasicEnemyPilot
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    TurretPilot();
    virtual ~TurretPilot();

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Update(float deltaSeconds, Ship* currentShip) override;
};