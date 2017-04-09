#pragma once
#include "Game/Encounters/Encounter.hpp"

class Wormhole;

//-----------------------------------------------------------------------------------
class WormholeEncounter : public Encounter
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    WormholeEncounter(const Vector2& center, float radius);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Spawn() override;
    virtual bool NeedsLinkedEncounter() { return true; };
    virtual Encounter* CreateLinkedEncounter(const Vector2& center, float radius);
    static void LinkWormholes(WormholeEncounter* wormhole1, WormholeEncounter* wormhole2);
    virtual bool IsWormhole() { return true; };

    ////MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    WormholeEncounter* m_linkedWormholeEncounter = nullptr;
    Wormhole* m_spawnedWormhole = nullptr;
};

