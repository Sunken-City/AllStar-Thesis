#pragma once
#include "Game/Items/Passives/PassiveEffect.hpp"
#include "Game/GameCommon.hpp"

class SharpshooterPassive : public PassiveEffect
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    SharpshooterPassive();
    virtual ~SharpshooterPassive() {};
    virtual void Update(float deltaSeconds) { UNUSED(deltaSeconds); };
    virtual void Activate(NamedProperties& parameters);
    virtual void Deactivate(NamedProperties& parameters);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
};
