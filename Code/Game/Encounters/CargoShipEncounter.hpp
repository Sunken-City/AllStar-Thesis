#pragma once
#include "Game/Encounters/Encounter.hpp"

//-----------------------------------------------------------------------------------
class CargoShipEncounter : public Encounter
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    CargoShipEncounter(const Vector2& center, float radius);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Spawn() override;

    ////MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};
