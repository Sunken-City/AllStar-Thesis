#pragma once
#include "Game/Items/Passives/PassiveEffect.hpp"

class CloakPassive : public PassiveEffect
{
public:
    //CONSTRUCTORS/////////////////////////////////////////////////////////////////////
    CloakPassive();
    virtual ~CloakPassive();
    virtual void Update(float deltaSeconds);
    virtual void Activate(NamedProperties& parameters);
    virtual void Deactivate(NamedProperties& parameters);

    //FUNCTIONS/////////////////////////////////////////////////////////////////////
    virtual const SpriteResource* GetSpriteResource();
};
