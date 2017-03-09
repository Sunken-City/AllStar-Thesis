#pragma once
#include "Game/Encounters/Encounter.hpp"

class BlackHole;

//-----------------------------------------------------------------------------------
class BlackHoleEncounter : public Encounter
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    BlackHoleEncounter(const Vector2& center, float radius);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Spawn() override;

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    BlackHole* m_spawnedBlackHole = nullptr;
};

