#pragma once
#include "Game/Encounters/Encounter.hpp"

//-----------------------------------------------------------------------------------
class SquadronEncounter : public Encounter
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    SquadronEncounter(const Vector2& center, float radius);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Spawn();

    ////MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};