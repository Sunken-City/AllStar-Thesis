#pragma once
#include "Game/Items/Passives/PassiveEffect.hpp"
#include "Game/GameCommon.hpp"

class SpecialTrailPassive : public PassiveEffect
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    SpecialTrailPassive();
    virtual ~SpecialTrailPassive() {};
    virtual void Update(float deltaSeconds) { UNUSED(deltaSeconds); };
    virtual void Activate(NamedProperties& parameters);
    virtual void Deactivate(NamedProperties& parameters);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();

    //MEMBER VARIABLES/////////////////////////////////////////////////////////////////////
    static const unsigned int MAX_TRAIL_IDS = 1;
    unsigned int m_trailID;
};