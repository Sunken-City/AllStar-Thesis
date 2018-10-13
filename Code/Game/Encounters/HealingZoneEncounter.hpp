#pragma once
#include "Game/Encounters/Encounter.hpp"

//-----------------------------------------------------------------------------------
class HealingZoneEncounter : public Encounter
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    HealingZoneEncounter(const Vector2& center, float radius);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Spawn() override;

    ////MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};
