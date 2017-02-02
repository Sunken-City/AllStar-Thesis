#pragma once
#include "Game/Items/Passives/PassiveEffect.hpp"
#include "Game/GameCommon.hpp"

class StealthTrailPassive : public PassiveEffect
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    StealthTrailPassive();
    virtual ~StealthTrailPassive() {};
    virtual void Update(float deltaSeconds) { UNUSED(deltaSeconds); };
    virtual void Activate(NamedProperties& parameters);
    virtual void Deactivate(NamedProperties& parameters);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
};