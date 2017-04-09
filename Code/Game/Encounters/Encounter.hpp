#pragma once
#include "Game/GameCommon.hpp"

//-----------------------------------------------------------------------------------
class Encounter
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    Encounter(const Vector2& center, float radius);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual void Spawn() = 0;
    virtual Vector2 CalculateSpawnPosition(const Vector2& relative01Position);
    virtual bool NeedsLinkedEncounter() { return false; };
    virtual Encounter* CreateLinkedEncounter(const Vector2& center, float radius) { UNUSED(center); UNUSED(radius); return nullptr; };
    virtual bool IsBlackHole() { return false; };
    virtual bool IsWormhole() { return false; };

    ////MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    Vector2 m_center;
    float m_radius;
};