#pragma once
#include "Game/Items/Passives/PassiveEffect.hpp"

class NoPassive : public PassiveEffect
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    NoPassive() {};
    virtual ~NoPassive() {};
    virtual void Update(float deltaSeconds) {};
    virtual void Activate(NamedProperties& parameters) {};
    virtual void Deactivate(NamedProperties& parameters) {};

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource() { return nullptr; };
};