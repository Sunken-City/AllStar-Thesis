#pragma once
#include "Game/Encounters/Encounter.hpp"

//-----------------------------------------------------------------------------------
class WormholeEncounter : public Encounter
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    WormholeEncounter(const Vector2& center, float radius);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Spawn() override;

    ////MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};

