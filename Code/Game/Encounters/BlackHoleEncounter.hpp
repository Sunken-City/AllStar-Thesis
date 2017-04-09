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
    virtual bool IsBlackHole() { return true; };

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    BlackHole* m_spawnedBlackHole = nullptr;
};

