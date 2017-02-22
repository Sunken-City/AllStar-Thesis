#pragma once
#include "Game/Encounters/Encounter.hpp"

//-----------------------------------------------------------------------------------
class BossteroidEncounter : public Encounter
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    BossteroidEncounter(const Vector2& center, float radius);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Spawn() override;

    ////MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
};

